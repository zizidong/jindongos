#include "cursor.h"
#include "gui.h"

cursor_t cursor = {400, 300, 1};

void cursor_init(void) {
    cursor.x = 400;
    cursor.y = 300;
    cursor.visible = 1;
}

void cursor_draw(void) {
    if (!cursor.visible) return;
    
    int32_t x = cursor.x;
    int32_t y = cursor.y;
    
    // Draw arrow cursor (classic Windows style)
    // Main triangle
    gui_draw_pixel(x, y, 0xffffff);
    gui_draw_pixel(x + 1, y + 1, 0xffffff);
    gui_draw_pixel(x, y + 1, 0xffffff);
    gui_draw_pixel(x + 2, y + 2, 0xffffff);
    gui_draw_pixel(x + 1, y + 2, 0xffffff);
    gui_draw_pixel(x, y + 2, 0xffffff);
    gui_draw_pixel(x + 3, y + 3, 0xffffff);
    gui_draw_pixel(x + 2, y + 3, 0xffffff);
    gui_draw_pixel(x + 1, y + 3, 0xffffff);
    gui_draw_pixel(x, y + 3, 0xffffff);
    gui_draw_pixel(x + 4, y + 4, 0xffffff);
    gui_draw_pixel(x + 3, y + 4, 0xffffff);
    gui_draw_pixel(x + 2, y + 4, 0xffffff);
    gui_draw_pixel(x + 1, y + 4, 0xffffff);
    gui_draw_pixel(x, y + 4, 0xffffff);
    gui_draw_pixel(x + 5, y + 5, 0xffffff);
    gui_draw_pixel(x + 4, y + 5, 0xffffff);
    gui_draw_pixel(x + 3, y + 5, 0xffffff);
    gui_draw_pixel(x + 2, y + 5, 0xffffff);
    gui_draw_pixel(x + 1, y + 5, 0xffffff);
    gui_draw_pixel(x, y + 5, 0xffffff);
    gui_draw_pixel(x + 6, y + 6, 0xffffff);
    gui_draw_pixel(x + 5, y + 6, 0xffffff);
    gui_draw_pixel(x + 4, y + 6, 0xffffff);
    gui_draw_pixel(x + 3, y + 6, 0xffffff);
    gui_draw_pixel(x + 2, y + 6, 0xffffff);
    gui_draw_pixel(x + 1, y + 6, 0xffffff);
    gui_draw_pixel(x, y + 6, 0xffffff);
    gui_draw_pixel(x + 7, y + 7, 0xffffff);
    gui_draw_pixel(x + 6, y + 7, 0xffffff);
    gui_draw_pixel(x + 5, y + 7, 0xffffff);
    gui_draw_pixel(x + 4, y + 7, 0xffffff);
    gui_draw_pixel(x + 3, y + 7, 0xffffff);
    gui_draw_pixel(x + 2, y + 7, 0xffffff);
    gui_draw_pixel(x + 1, y + 7, 0xffffff);
    gui_draw_pixel(x, y + 7, 0xffffff);
    gui_draw_pixel(x + 8, y + 8, 0xffffff);
    gui_draw_pixel(x + 7, y + 8, 0xffffff);
    gui_draw_pixel(x + 6, y + 8, 0xffffff);
    gui_draw_pixel(x + 5, y + 8, 0xffffff);
    gui_draw_pixel(x + 4, y + 8, 0xffffff);
    gui_draw_pixel(x + 3, y + 8, 0xffffff);
    gui_draw_pixel(x + 2, y + 8, 0xffffff);
    gui_draw_pixel(x + 1, y + 8, 0xffffff);
    gui_draw_pixel(x, y + 8, 0xffffff);
    
    // Black outline
    gui_draw_pixel(x + 1, y, 0x000000);
    gui_draw_pixel(x + 2, y + 1, 0x000000);
    gui_draw_pixel(x + 3, y + 2, 0x000000);
    gui_draw_pixel(x + 4, y + 3, 0x000000);
    gui_draw_pixel(x + 5, y + 4, 0x000000);
    gui_draw_pixel(x + 6, y + 5, 0x000000);
    gui_draw_pixel(x + 7, y + 6, 0x000000);
    gui_draw_pixel(x + 8, y + 7, 0x000000);
    gui_draw_pixel(x + 9, y + 8, 0x000000);
}

void cursor_update(int32_t x, int32_t y) {
    cursor.x = x;
    cursor.y = y;
}
