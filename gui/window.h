#ifndef _WINDOW_H
#define _WINDOW_H

#include <stdint.h>

#define MAX_WINDOWS 10

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    char title[64];
    uint32_t bg_color;
    uint8_t visible;
} window_t;

typedef struct {
    window_t windows[MAX_WINDOWS];
    uint32_t window_count;
    int32_t focused_window;
} window_manager_t;

extern window_manager_t wm;

void window_manager_init(void);
int32_t window_create(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const char *title);
void window_draw(int32_t window_id);
void window_draw_all(void);

#endif
