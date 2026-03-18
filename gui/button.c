#include "button.h"
#include "gui.h"

button_manager_t bm = {0};

void button_manager_init(void) {
    bm.button_count = 0;
}

int32_t button_create(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const char *label) {
    if (bm.button_count >= MAX_BUTTONS) return -1;
    
    int32_t id = bm.button_count;
    button_t *btn = &bm.buttons[id];
    
    btn->x = x;
    btn->y = y;
    btn->width = w;
    btn->height = h;
    btn->bg_color = 0x0066cc;
    btn->hover_color = 0x0088ff;
    btn->hovered = 0;
    
    for (int i = 0; label[i] && i < 31; i++) {
        btn->label[i] = label[i];
    }
    btn->label[31] = 0;
    
    bm.button_count++;
    return id;
}

void button_draw(int32_t button_id) {
    if (button_id < 0 || (uint32_t)button_id >= bm.button_count) return;
    
    button_t *btn = &bm.buttons[button_id];
    uint32_t color = btn->hovered ? btn->hover_color : btn->bg_color;
    
    gui_draw_gradient(btn->x, btn->y, btn->width, btn->height, color, gui_blend_colors(color, 0x000000, 50));
    
    // Draw border
    gui_draw_rect(btn->x, btn->y, btn->width, 1, 0xffffff);
    gui_draw_rect(btn->x, btn->y + btn->height - 1, btn->width, 1, 0x000000);
    gui_draw_rect(btn->x, btn->y, 1, btn->height, 0xffffff);
    gui_draw_rect(btn->x + btn->width - 1, btn->y, 1, btn->height, 0x000000);
}

void button_draw_all(void) {
    for (uint32_t i = 0; i < bm.button_count; i++) {
        button_draw(i);
    }
}

void button_update_hover(int32_t mouse_x, int32_t mouse_y) {
    for (uint32_t i = 0; i < bm.button_count; i++) {
        button_t *btn = &bm.buttons[i];
        btn->hovered = ((uint32_t)mouse_x >= btn->x && (uint32_t)mouse_x < btn->x + btn->width &&
                        (uint32_t)mouse_y >= btn->y && (uint32_t)mouse_y < btn->y + btn->height);
    }
}
