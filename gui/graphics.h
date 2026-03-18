#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <stdint.h>

typedef struct {
    uint32_t color1;
    uint32_t color2;
    uint8_t direction;
} gradient_t;

void graphics_draw_gradient(uint32_t x, uint32_t y, uint32_t width, uint32_t height, gradient_t *grad);
void graphics_draw_blur_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color, uint8_t intensity);
void graphics_draw_rounded_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t radius, uint32_t color);
uint32_t graphics_blend_colors(uint32_t color1, uint32_t color2, uint8_t alpha);

#endif
