#include "gtk_ui.h"
#include "gui.h"

theme_t gtk_theme = {
    .bg = 0xf0f0f0,
    .fg = 0x000000,
    .border = 0xcccccc,
    .hover = 0xe0e0e0,
    .active = 0x0066cc
};

void gtk_init(void) {
    gtk_theme.bg = 0xf0f0f0;
    gtk_theme.fg = 0x000000;
    gtk_theme.border = 0xcccccc;
    gtk_theme.hover = 0xe0e0e0;
    gtk_theme.active = 0x0066cc;
}

void gtk_draw_button(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const char *label, uint8_t hovered) {
    uint32_t bg_color = hovered ? gtk_theme.hover : gtk_theme.bg;
    
    gui_draw_rect(x, y, w, h, bg_color);
    gui_draw_rect(x, y, w, 1, 0xffffff);
    gui_draw_rect(x, y + h - 1, w, 1, 0x999999);
    gui_draw_rect(x, y, 1, h, 0xffffff);
    gui_draw_rect(x + w - 1, y, 1, h, 0x999999);
    
    gui_draw_text(x + 8, y + 8, label, gtk_theme.fg);
}

void gtk_draw_entry(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const char *text) {
    gui_draw_rect(x, y, w, h, 0xffffff);
    gui_draw_rect(x, y, w, 1, 0x999999);
    gui_draw_rect(x, y + h - 1, w, 1, 0xcccccc);
    gui_draw_rect(x, y, 1, h, 0x999999);
    gui_draw_rect(x + w - 1, y, 1, h, 0xcccccc);
    
    gui_draw_text(x + 4, y + 4, text, gtk_theme.fg);
}

void gtk_draw_label(uint32_t x, uint32_t y, const char *text) {
    gui_draw_text(x, y, text, gtk_theme.fg);
}

void gtk_draw_panel(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    gui_draw_rect(x, y, w, h, gtk_theme.bg);
    gui_draw_rect(x, y, w, 1, 0xffffff);
    gui_draw_rect(x, y + h - 1, w, 1, 0x999999);
    gui_draw_rect(x, y, 1, h, 0xffffff);
    gui_draw_rect(x + w - 1, y, 1, h, 0x999999);
}

void gtk_draw_separator(uint32_t x, uint32_t y, uint32_t w) {
    gui_draw_rect(x, y, w, 1, 0xcccccc);
    gui_draw_rect(x, y + 1, w, 1, 0xffffff);
}

void gtk_draw_checkbox(uint32_t x, uint32_t y, uint8_t checked) {
    gui_draw_rect(x, y, 16, 16, 0xffffff);
    gui_draw_rect(x, y, 16, 1, 0x999999);
    gui_draw_rect(x, y + 15, 16, 1, 0xcccccc);
    gui_draw_rect(x, y, 1, 16, 0x999999);
    gui_draw_rect(x + 15, y, 1, 16, 0xcccccc);
    
    if (checked) {
        for (uint32_t i = 4; i < 12; i++) {
            gui_draw_pixel(x + i, y + i - 2, gtk_theme.active);
            gui_draw_pixel(x + i, y + i - 1, gtk_theme.active);
        }
    }
}

void gtk_draw_progressbar(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t progress) {
    gui_draw_rect(x, y, w, h, 0xffffff);
    gui_draw_rect(x, y, w, 1, 0x999999);
    gui_draw_rect(x, y + h - 1, w, 1, 0xcccccc);
    gui_draw_rect(x, y, 1, h, 0x999999);
    gui_draw_rect(x + w - 1, y, 1, h, 0xcccccc);
    
    uint32_t filled = (w * progress) / 100;
    gui_draw_rect(x + 1, y + 1, filled - 2, h - 2, gtk_theme.active);
}
