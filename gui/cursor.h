#ifndef _CURSOR_H
#define _CURSOR_H

#include <stdint.h>

typedef struct {
    int32_t x;
    int32_t y;
    uint8_t visible;
} cursor_t;

extern cursor_t cursor;

void cursor_init(void);
void cursor_draw(void);
void cursor_update(int32_t x, int32_t y);

#endif
