#include "mouse.h"
#include "../gui/gtk.h"

int32_t mouse_x        = 400;
int32_t mouse_y        = 300;
uint8_t mouse_buttons  = 0;
uint8_t mouse_prev_buttons = 0;

/* ── PS/2 포트 ── */
#define PS2_DATA   0x60
#define PS2_STATUS 0x64
#define PS2_CMD    0x64

static inline uint8_t inb(uint16_t port) {
    uint8_t v;
    __asm__ volatile("inb %1, %0" : "=a"(v) : "Nd"(port));
    return v;
}
static inline void outb(uint16_t port, uint8_t v) {
    __asm__ volatile("outb %0, %1" :: "a"(v), "Nd"(port));
}
static inline void io_wait(void) {
    __asm__ volatile("outb %0, $0x80" :: "a"((uint8_t)0));
}

/* 화면 크기 (mouse_init에서 설정) */
static int32_t scr_w = 1024;
static int32_t scr_h = 768;

void mouse_init(void) {
    mouse_x = scr_w / 2;
    mouse_y = scr_h / 2;
    mouse_buttons = 0;
    mouse_prev_buttons = 0;
    /* PS/2 마우스는 QEMU에서 자동 활성화됨 */
}

void mouse_set_screen(int32_t w, int32_t h) {
    scr_w = w; scr_h = h;
}

/* 3바이트 패킷 상태 머신 */
static uint8_t pkt[3];
static int     pkt_idx = 0;

void mouse_poll(void) {
    /* PS/2 상태: bit0=output buffer full, bit5=aux data */
    while (inb(PS2_STATUS) & 0x01) {
        uint8_t byte = inb(PS2_DATA);

        if (pkt_idx == 0) {
            if (!(byte & 0x08)) continue;  /* 동기화 비트 확인 */
        }

        pkt[pkt_idx++] = byte;

        if (pkt_idx == 3) {
            pkt_idx = 0;

            int8_t dx =  (int8_t)pkt[1];
            int8_t dy = -(int8_t)pkt[2];

            mouse_buttons = pkt[0] & 0x07;
            mouse_x += dx;
            mouse_y += dy;

            if (mouse_x < 0)       mouse_x = 0;
            if (mouse_y < 0)       mouse_y = 0;
            if (mouse_x >= scr_w)  mouse_x = scr_w - 1;
            if (mouse_y >= scr_h)  mouse_y = scr_h - 1;

            GtkMouseEvent ev;
            ev.x       = mouse_x;
            ev.y       = mouse_y;
            ev.buttons = mouse_buttons;
            ev.dx      = dx;
            ev.dy      = (int8_t)pkt[2];
            gtk_push_mouse_event(&ev);
        }
    }
}
