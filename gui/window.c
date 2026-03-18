#include "window.h"
#include "gui.h"

window_manager_t wm = {0};

void window_manager_init(void) {
    wm.window_count = 0;
    wm.focused_window = -1;
}

int32_t window_create(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const char *title) {
    if (wm.window_count >= MAX_WINDOWS) return -1;
    
    int32_t id = wm.window_count;
    window_t *win = &wm.windows[id];
    
    win->x = x;
    win->y = y;
    win->width = w;
    win->height = h;
    win->bg_color = 0x2a2a2a;
    win->visible = 1;
    
    for (int i = 0; title[i] && i < 63; i++) {
        win->title[i] = title[i];
    }
    win->title[63] = 0;
    
    wm.window_count++;
    wm.focused_window = id;
    
    return id;
}

void window_draw(int32_t window_id) {
    if (window_id < 0 || (uint32_t)window_id >= wm.window_count) return;
    
    window_t *win = &wm.windows[window_id];
    if (!win->visible) return;
    
    uint32_t border_color = (window_id == wm.focused_window) ? 0x0088ff : 0x666666;
    uint32_t title_color = (window_id == wm.focused_window) ? 0x0066cc : 0x444444;
    
    // Draw shadow
    gui_draw_rect(win->x + 2, win->y + 2, win->width, win->height, 0x000000);
    
    // Draw border
    gui_draw_rect(win->x, win->y, win->width, win->height, border_color);
    
    // Draw title bar with gradient
    gui_draw_gradient(win->x + 1, win->y + 1, win->width - 2, 25, 0x0066cc, 0x004499);
    
    // Draw client area
    gui_draw_rect(win->x + 1, win->y + 26, win->width - 2, win->height - 27, win->bg_color);
}

void window_draw_all(void) {
    for (uint32_t i = 0; i < wm.window_count; i++) {
        window_draw(i);
    }
}
