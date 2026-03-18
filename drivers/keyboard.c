#include "keyboard.h"
#include "../gui/gtk.h"

keyboard_event_t last_key = {0, 0, 0};

#define PS2_DATA   0x60
#define PS2_STATUS 0x64

static inline uint8_t inb(uint16_t port) {
    uint8_t v;
    __asm__ volatile("inb %1,%0" : "=a"(v) : "Nd"(port));
    return v;
}

/* scancode set 1 → ASCII (unshifted) */
static const char sc_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']', '\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',  0, '\\',
    'z','x','c','v','b','n','m',',','.','/',  0, '*',  0, ' ',
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

void keyboard_init(void) {
    last_key.scancode = 0;
    last_key.ascii    = 0;
    last_key.pressed  = 0;
}

void keyboard_poll(void) {
    /* 키보드 데이터만 (aux 비트=0) */
    while ((inb(PS2_STATUS) & 0x21) == 0x01) {
        uint8_t sc      = inb(PS2_DATA);
        uint8_t pressed = !(sc & 0x80);
        uint8_t key     = sc & 0x7F;
        char    ascii   = (key < 128) ? sc_ascii[key] : 0;

        last_key.scancode = key;
        last_key.ascii    = ascii;
        last_key.pressed  = pressed;

        GtkKeyEvent kev;
        kev.scancode = key;
        kev.ascii    = ascii;
        kev.pressed  = pressed;
        gtk_push_key_event(&kev);
    }
}
