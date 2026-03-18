#ifndef _GTK_UI_H
#define _GTK_UI_H

#include <stdint.h>

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} rect_t;

typedef struct {
    uint32_t bg;
    uint32_t fg;
    uint32_t border;
    uint32_t hover;
    uint32_t active;
} theme_t;

extern theme_t gtk_theme;

void gtk_init(void);
void gtk_draw_button(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const char *label, uint8_t hovered);
void gtk_draw_entry(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const char *text);
void gtk_draw_label(uint32_t x, uint32_t y, const char *text);
void gtk_draw_panel(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void gtk_draw_separator(uint32_t x, uint32_t y, uint32_t w);
void gtk_draw_checkbox(uint32_t x, uint32_t y, uint8_t checked);
void gtk_draw_progressbar(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t progress);

#endif
