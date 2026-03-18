#include <stdint.h>
#include <stddef.h>
#include "limine.h"
#include "libc.h"
#include "../drivers/mouse.h"
#include "../drivers/keyboard.h"
#include "../gui/gtk.h"
#include "thread.h"

__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(1);

__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request fb_req = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests")))
static volatile struct limine_memmap_request mm_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

/* ── 힙 ── */
static void heap_init(void) {
    if (!mm_req.response) return;
    for (uint64_t i = 0; i < mm_req.response->entry_count; i++) {
        struct limine_memmap_entry *e = mm_req.response->entries[i];
        if (e->type == LIMINE_MEMMAP_USABLE && e->length >= 4 * 1024 * 1024 && e->base >= 0x100000) {
            _heap_base = (uint8_t *)e->base;
            _heap_size = e->length;
            return;
        }
    }
}


/* ── 디스플레이 ── */
typedef struct { void *addr; uint64_t w, h, pitch; } display_t;
static display_t disp;
static uint32_t *back_buf = NULL;

static void swap_buffers(void) {
    uint32_t *fb     = (uint32_t *)disp.addr;
    uint32_t  stride = (uint32_t)(disp.pitch / 4);
    for (uint32_t r = 0; r < disp.h; r++) {
        uint32_t *src = back_buf + r * stride;
        uint32_t *dst = fb      + r * stride;
        for (uint32_t c = 0; c < disp.w; c++)
            dst[c] = src[c];
    }
}

void draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= disp.w || y >= disp.h) return;
    back_buf[y * (disp.pitch / 4) + x] = color;
}

void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    uint32_t stride = (uint32_t)(disp.pitch / 4);
    for (uint32_t r = y; r < y + h && r < disp.h; r++) {
        uint32_t *row = back_buf + r * stride;
        for (uint32_t c = x; c < x + w && c < disp.w; c++)
            row[c] = color;
    }
}

void draw_border(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    draw_rect(x,     y,     w, 1, color);
    draw_rect(x,     y+h-1, w, 1, color);
    draw_rect(x,     y,     1, h, color);
    draw_rect(x+w-1, y,     1, h, color);
}

/* ── 커서 ── */
static const uint8_t cursor_bmp[19][12] = {
    {1,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0},
    {1,2,1,0,0,0,0,0,0,0,0,0},
    {1,2,2,1,0,0,0,0,0,0,0,0},
    {1,2,2,2,1,0,0,0,0,0,0,0},
    {1,2,2,2,2,1,0,0,0,0,0,0},
    {1,2,2,2,2,2,1,0,0,0,0,0},
    {1,2,2,2,2,2,2,1,0,0,0,0},
    {1,2,2,2,2,2,2,2,1,0,0,0},
    {1,2,2,2,2,2,2,2,2,1,0,0},
    {1,2,2,2,2,2,2,2,2,2,1,0},
    {1,2,2,2,2,2,2,1,1,1,1,1},
    {1,2,2,2,1,2,2,1,0,0,0,0},
    {1,2,2,1,0,1,2,2,1,0,0,0},
    {1,2,1,0,0,1,2,2,1,0,0,0},
    {1,1,0,0,0,0,1,2,2,1,0,0},
    {0,0,0,0,0,0,1,2,2,1,0,0},
    {0,0,0,0,0,0,0,1,2,1,0,0},
    {0,0,0,0,0,0,0,1,1,0,0,0},
};

static void draw_cursor(int32_t x, int32_t y) {
    if (x < 0 || y < 0) return;
    for (int row = 0; row < 19; row++)
        for (int col = 0; col < 12; col++) {
            uint8_t p = cursor_bmp[row][col];
            if (p == 1) draw_pixel((uint32_t)(x+col), (uint32_t)(y+row), 0x000000);
            else if (p == 2) draw_pixel((uint32_t)(x+col), (uint32_t)(y+row), 0xFFFFFF);
        }
}

/* ── 콜백 ── */
static GtkWidget *status_label = NULL;

static void on_ok_clicked(void *w, void *d)     { (void)w; (void)d; if (status_label) gtk_label_set_text(status_label, "OK clicked!"); }
static void on_cancel_clicked(void *w, void *d) { (void)w; (void)d; if (status_label) gtk_label_set_text(status_label, "Cancelled."); }
static void on_about_clicked(void *w, void *d)  { (void)w; (void)d; if (status_label) gtk_label_set_text(status_label, "MyOS v2.0 - UEFI GTK"); }

static void wait_60hz(void) {
    for (volatile uint32_t i = 0; i < 2000000; i++);
}

void _start(void) {
    if (LIMINE_BASE_REVISION_SUPPORTED != 0)
        for (;;) __asm__("hlt");

    if (!fb_req.response || fb_req.response->framebuffer_count < 1)
        for (;;) __asm__("hlt");

    struct limine_framebuffer *fb = fb_req.response->framebuffers[0];
    disp.addr  = fb->address;
    disp.w     = fb->width;
    disp.h     = fb->height;
    disp.pitch = fb->pitch;

    heap_init();
    back_buf = (uint32_t *)malloc(disp.pitch * disp.h);
    if (!back_buf) for (;;) __asm__("hlt");

    mouse_set_screen((int32_t)disp.w, (int32_t)disp.h);
    mouse_init();
    keyboard_init();

    int argc = 0;
    char *argv_arr[] = {0};
    char **argv = argv_arr;
    gtk_init(&argc, &argv);

    GtkWindow *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(win, "MyOS - GTK Desktop");
    gtk_window_set_default_size(win, 620, 400);
    win->x = (uint32_t)(disp.w / 2 - 310);
    win->y = (uint32_t)(disp.h / 2 - 200);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    win->root = vbox;

    GtkWidget *heading = gtk_label_new("Welcome to MyOS GTK Desktop");
    gtk_box_pack_start(vbox, heading, 0, 0, 0);

    GtkWidget *sep1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    sep1->w = 580;
    gtk_box_pack_start(vbox, sep1, 0, 0, 0);

    GtkWidget *lbl_name = gtk_label_new("Name:");
    gtk_box_pack_start(vbox, lbl_name, 0, 0, 0);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_text(entry, "Type here...");
    entry->w = 300;
    gtk_box_pack_start(vbox, entry, 0, 0, 0);

    GtkWidget *chk1 = gtk_check_button_new_with_label("Enable networking");
    gtk_box_pack_start(vbox, chk1, 0, 0, 0);

    GtkWidget *chk2 = gtk_check_button_new_with_label("Enable sound");
    gtk_toggle_button_set_active(chk2, 1);
    gtk_box_pack_start(vbox, chk2, 0, 0, 0);

    GtkWidget *lbl_prog = gtk_label_new("System load:");
    gtk_box_pack_start(vbox, lbl_prog, 0, 0, 0);

    GtkWidget *progress = gtk_progress_bar_new();
    progress->w = 300;
    gtk_progress_bar_set_fraction(progress, 42);
    gtk_box_pack_start(vbox, progress, 0, 0, 0);

    GtkWidget *sep2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    sep2->w = 580;
    gtk_box_pack_start(vbox, sep2, 0, 0, 0);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_pack_start(vbox, hbox, 0, 0, 0);

    GtkWidget *btn_ok = gtk_button_new_with_label("OK");
    gtk_signal_connect(btn_ok, "clicked", on_ok_clicked, NULL);
    gtk_box_pack_start(hbox, btn_ok, 0, 0, 0);

    GtkWidget *btn_cancel = gtk_button_new_with_label("Cancel");
    gtk_signal_connect(btn_cancel, "clicked", on_cancel_clicked, NULL);
    gtk_box_pack_start(hbox, btn_cancel, 0, 0, 0);

    GtkWidget *btn_about = gtk_button_new_with_label("About");
    gtk_signal_connect(btn_about, "clicked", on_about_clicked, NULL);
    gtk_box_pack_start(hbox, btn_about, 0, 0, 0);

    status_label = gtk_label_new("Ready.");
    gtk_box_pack_start(vbox, status_label, 0, 0, 0);

    gtk_window_show_all(win);

    while (1) {
        wait_60hz();
        mouse_poll();
        keyboard_poll();
        gtk_poll_events(win);

        draw_rect(0, 0, (uint32_t)disp.w, (uint32_t)disp.h, 0x2E6DA4);
        gtk_render_window(win);
        draw_cursor(mouse_x, mouse_y);
        swap_buffers();
    }
}
