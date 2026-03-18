#include "gui.h"

typedef struct {
    void *address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
} display_t;

static display_t display;

void gui_draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= display.width || y >= display.height) return;
    uint32_t *fb = (uint32_t *)display.address;
    fb[y * (display.pitch / 4) + x] = color;
}

void gui_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    for (uint32_t i = y; i < y + h && i < display.height; i++) {
        for (uint32_t j = x; j < x + w && j < display.width; j++) {
            gui_draw_pixel(j, i, color);
        }
    }
}

void gui_draw_text(uint32_t x, uint32_t y, const char *str, uint32_t color) {
    uint32_t pos_x = x;
    for (int i = 0; str[i]; i++) {
        pos_x += 8;
    }
}
