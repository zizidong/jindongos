#ifndef _BUFFER_H
#define _BUFFER_H

#include <stdint.h>

void buffer_init(void *fb, uint64_t width, uint64_t height, uint64_t pitch);
void buffer_clear(uint32_t color);
void buffer_draw_pixel(uint32_t x, uint32_t y, uint32_t color);
void buffer_swap(void);

#endif
