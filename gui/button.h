#ifndef _BUTTON_H
#define _BUTTON_H

#include <stdint.h>

#define MAX_BUTTONS 50

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    char label[32];
    uint32_t bg_color;
    uint32_t hover_color;
    uint8_t hovered;
} button_t;

typedef struct {
    button_t buttons[MAX_BUTTONS];
    uint32_t button_count;
} button_manager_t;

extern button_manager_t bm;

void button_manager_init(void);
int32_t button_create(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const char *label);
void button_draw(int32_t button_id);
void button_draw_all(void);
void button_update_hover(int32_t mouse_x, int32_t mouse_y);

#endif
