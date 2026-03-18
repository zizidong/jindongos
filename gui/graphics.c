#include "graphics.h"
#include "gui.h"

uint32_t graphics_blend_colors(uint32_t color1, uint32_t color2, uint8_t alpha) {
    uint8_t r1 = (color1 >> 16) & 0xFF;
    uint8_t g1 = (color1 >> 8) & 0xFF;
    uint8_t b1 = color1 & 0xFF;
    
    uint8_t r2 = (color2 >> 16) & 0xFF;
    uint8_t g2 = (color2 >> 8) & 0xFF;
    uint8_t b2 = color2 & 0xFF;
    
    uint8_t r = (r1 * (255 - alpha) + r2 * alpha) / 255;
    uint8_t g = (g1 * (255 - alpha) + g2 * alpha) / 255;
    uint8_t b = (b1 * (255 - alpha) + b2 * alpha) / 255;
    
    return (r << 16) | (g << 8) | b;
}

void graphics_draw_gradient(uint32_t x, uint32_t y, uint32_t width, uint32_t height, gradient_t *grad) {
    for (uint32_t i = 0; i < height; i++) {
        uint8_t alpha = (i * 255) / height;
        uint32_t color = graphics_blend_colors(grad->color1, grad->color2, alpha);
        
        for (uint32_t j = 0; j < width; j++) {
            gui_draw_pixel(x + j, y + i, color);
        }
    }
}

void graphics_draw_blur_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color, uint8_t intensity) {
    uint8_t alpha_step = 255 / (intensity + 1);
    
    for (uint8_t layer = 0; layer <= intensity; layer++) {
        uint8_t alpha = alpha_step * (intensity - layer);
        uint32_t blurred_color = graphics_blend_colors(0x1a1a1a, color, alpha);
        
        uint32_t offset = layer;
        for (uint32_t i = 0; i < height; i++) {
            for (uint32_t j = 0; j < width; j++) {
                if (x + j + offset < display.width && y + i + offset < display.height) {
                    gui_draw_pixel(x + j + offset, y + i + offset, blurred_color);
                }
            }
        }
    }
}

void graphics_draw_rounded_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t radius, uint32_t color) {
    int32_t r = (int32_t)radius;
    rect_t main = {x + radius, y, width - 2 * radius, height, color};
    gui_draw_rect(&main);
    
    rect_t side1 = {x, y + radius, radius, height - 2 * radius, color};
    gui_draw_rect(&side1);
    
    rect_t side2 = {x + width - radius, y + radius, radius, height - 2 * radius, color};
    gui_draw_rect(&side2);
    
    for (int32_t i = -r; i <= r; i++) {
        for (int32_t j = -r; j <= r; j++) {
            if (i * i + j * j <= r * r) {
                gui_draw_pixel(x + radius + i, y + radius + j, color);
                gui_draw_pixel(x + width - radius + i, y + radius + j, color);
                gui_draw_pixel(x + radius + i, y + height - radius + j, color);
                gui_draw_pixel(x + width - radius + i, y + height - radius + j, color);
            }
        }
    }
}
