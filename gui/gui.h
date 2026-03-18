#ifndef _GUI_H
#define _GUI_H

#include <stdint.h>

void gui_draw_pixel(uint32_t x, uint32_t y, uint32_t color);
void gui_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void gui_draw_text(uint32_t x, uint32_t y, const char *str, uint32_t color);

#endif
