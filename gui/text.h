#ifndef _TEXT_H
#define _TEXT_H

#include <stdint.h>

void text_draw_char(uint32_t x, uint32_t y, char c, uint32_t color);
void text_draw_string(uint32_t x, uint32_t y, const char *str, uint32_t color);

#endif
