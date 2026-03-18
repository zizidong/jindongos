#include "gtk.h"
#include <stddef.h>

/* ── framebuffer helpers (defined in kernel.c) ── */
extern void draw_pixel(uint32_t x, uint32_t y, uint32_t color);
extern void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
extern void draw_border(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

/* ── colours ── */
#define COL_BG          0xF0F0F0
#define COL_WINDOW_BG   0xECECEC
#define COL_TITLEBAR    0x3C6EB4
#define COL_TITLEBAR_TXT 0xFFFFFF
#define COL_BUTTON_BG   0xDDDDDD
#define COL_BUTTON_HOV  0xC8E0FF
#define COL_BUTTON_BRD  0x999999
#define COL_ENTRY_BG    0xFFFFFF
#define COL_ENTRY_BRD   0x7A9CC6
#define COL_CHECK_BG    0xFFFFFF
#define COL_CHECK_BRD   0x888888
#define COL_CHECK_TICK  0x2266CC
#define COL_PROGRESS_BG 0xDDDDDD
#define COL_PROGRESS_FG 0x3C9E3C
#define COL_SEP         0xBBBBBB
#define COL_TEXT        0x111111

/* ── 5×7 bitmap font (ASCII 32-126) ── */
static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, /* 32 space */
    {0x00,0x00,0x5F,0x00,0x00}, /* 33 ! */
    {0x00,0x07,0x00,0x07,0x00}, /* 34 " */
    {0x14,0x7F,0x14,0x7F,0x14}, /* 35 # */
    {0x24,0x2A,0x7F,0x2A,0x12}, /* 36 $ */
    {0x23,0x13,0x08,0x64,0x62}, /* 37 % */
    {0x36,0x49,0x55,0x22,0x50}, /* 38 & */
    {0x00,0x05,0x03,0x00,0x00}, /* 39 ' */
    {0x00,0x1C,0x22,0x41,0x00}, /* 40 ( */
    {0x00,0x41,0x22,0x1C,0x00}, /* 41 ) */
    {0x14,0x08,0x3E,0x08,0x14}, /* 42 * */
    {0x08,0x08,0x3E,0x08,0x08}, /* 43 + */
    {0x00,0x50,0x30,0x00,0x00}, /* 44 , */
    {0x08,0x08,0x08,0x08,0x08}, /* 45 - */
    {0x00,0x60,0x60,0x00,0x00}, /* 46 . */
    {0x20,0x10,0x08,0x04,0x02}, /* 47 / */
    {0x3E,0x51,0x49,0x45,0x3E}, /* 48 0 */
    {0x00,0x42,0x7F,0x40,0x00}, /* 49 1 */
    {0x42,0x61,0x51,0x49,0x46}, /* 50 2 */
    {0x21,0x41,0x45,0x4B,0x31}, /* 51 3 */
    {0x18,0x14,0x12,0x7F,0x10}, /* 52 4 */
    {0x27,0x45,0x45,0x45,0x39}, /* 53 5 */
    {0x3C,0x4A,0x49,0x49,0x30}, /* 54 6 */
    {0x01,0x71,0x09,0x05,0x03}, /* 55 7 */
    {0x36,0x49,0x49,0x49,0x36}, /* 56 8 */
    {0x06,0x49,0x49,0x29,0x1E}, /* 57 9 */
    {0x00,0x36,0x36,0x00,0x00}, /* 58 : */
    {0x00,0x56,0x36,0x00,0x00}, /* 59 ; */
    {0x08,0x14,0x22,0x41,0x00}, /* 60 < */
    {0x14,0x14,0x14,0x14,0x14}, /* 61 = */
    {0x00,0x41,0x22,0x14,0x08}, /* 62 > */
    {0x02,0x01,0x51,0x09,0x06}, /* 63 ? */
    {0x32,0x49,0x79,0x41,0x3E}, /* 64 @ */
    {0x7E,0x11,0x11,0x11,0x7E}, /* 65 A */
    {0x7F,0x49,0x49,0x49,0x36}, /* 66 B */
    {0x3E,0x41,0x41,0x41,0x22}, /* 67 C */
    {0x7F,0x41,0x41,0x22,0x1C}, /* 68 D */
    {0x7F,0x49,0x49,0x49,0x41}, /* 69 E */
    {0x7F,0x09,0x09,0x09,0x01}, /* 70 F */
    {0x3E,0x41,0x49,0x49,0x7A}, /* 71 G */
    {0x7F,0x08,0x08,0x08,0x7F}, /* 72 H */
    {0x00,0x41,0x7F,0x41,0x00}, /* 73 I */
    {0x20,0x40,0x41,0x3F,0x01}, /* 74 J */
    {0x7F,0x08,0x14,0x22,0x41}, /* 75 K */
    {0x7F,0x40,0x40,0x40,0x40}, /* 76 L */
    {0x7F,0x02,0x0C,0x02,0x7F}, /* 77 M */
    {0x7F,0x04,0x08,0x10,0x7F}, /* 78 N */
    {0x3E,0x41,0x41,0x41,0x3E}, /* 79 O */
    {0x7F,0x09,0x09,0x09,0x06}, /* 80 P */
    {0x3E,0x41,0x51,0x21,0x5E}, /* 81 Q */
    {0x7F,0x09,0x19,0x29,0x46}, /* 82 R */
    {0x46,0x49,0x49,0x49,0x31}, /* 83 S */
    {0x01,0x01,0x7F,0x01,0x01}, /* 84 T */
    {0x3F,0x40,0x40,0x40,0x3F}, /* 85 U */
    {0x1F,0x20,0x40,0x20,0x1F}, /* 86 V */
    {0x3F,0x40,0x38,0x40,0x3F}, /* 87 W */
    {0x63,0x14,0x08,0x14,0x63}, /* 88 X */
    {0x07,0x08,0x70,0x08,0x07}, /* 89 Y */
    {0x61,0x51,0x49,0x45,0x43}, /* 90 Z */
    {0x00,0x7F,0x41,0x41,0x00}, /* 91 [ */
    {0x02,0x04,0x08,0x10,0x20}, /* 92 \ */
    {0x00,0x41,0x41,0x7F,0x00}, /* 93 ] */
    {0x04,0x02,0x01,0x02,0x04}, /* 94 ^ */
    {0x40,0x40,0x40,0x40,0x40}, /* 95 _ */
    {0x00,0x01,0x02,0x04,0x00}, /* 96 ` */
    {0x20,0x54,0x54,0x54,0x78}, /* 97 a */
    {0x7F,0x48,0x44,0x44,0x38}, /* 98 b */
    {0x38,0x44,0x44,0x44,0x20}, /* 99 c */
    {0x38,0x44,0x44,0x48,0x7F}, /* 100 d */
    {0x38,0x54,0x54,0x54,0x18}, /* 101 e */
    {0x08,0x7E,0x09,0x01,0x02}, /* 102 f */
    {0x0C,0x52,0x52,0x52,0x3E}, /* 103 g */
    {0x7F,0x08,0x04,0x04,0x78}, /* 104 h */
    {0x00,0x44,0x7D,0x40,0x00}, /* 105 i */
    {0x20,0x40,0x44,0x3D,0x00}, /* 106 j */
    {0x7F,0x10,0x28,0x44,0x00}, /* 107 k */
    {0x00,0x41,0x7F,0x40,0x00}, /* 108 l */
    {0x7C,0x04,0x18,0x04,0x78}, /* 109 m */
    {0x7C,0x08,0x04,0x04,0x78}, /* 110 n */
    {0x38,0x44,0x44,0x44,0x38}, /* 111 o */
    {0x7C,0x14,0x14,0x14,0x08}, /* 112 p */
    {0x08,0x14,0x14,0x18,0x7C}, /* 113 q */
    {0x7C,0x08,0x04,0x04,0x08}, /* 114 r */
    {0x48,0x54,0x54,0x54,0x20}, /* 115 s */
    {0x04,0x3F,0x44,0x40,0x20}, /* 116 t */
    {0x3C,0x40,0x40,0x20,0x7C}, /* 117 u */
    {0x1C,0x20,0x40,0x20,0x1C}, /* 118 v */
    {0x3C,0x40,0x30,0x40,0x3C}, /* 119 w */
    {0x44,0x28,0x10,0x28,0x44}, /* 120 x */
    {0x0C,0x50,0x50,0x50,0x3C}, /* 121 y */
    {0x44,0x64,0x54,0x4C,0x44}, /* 122 z */
    {0x00,0x08,0x36,0x41,0x00}, /* 123 { */
    {0x00,0x00,0x7F,0x00,0x00}, /* 124 | */
    {0x00,0x41,0x36,0x08,0x00}, /* 125 } */
    {0x10,0x08,0x08,0x10,0x08}, /* 126 ~ */
};

/* ── string helpers ── */
static int gtk_strlen(const char *s) {
    int n = 0;
    while (s && s[n]) n++;
    return n;
}

static void gtk_strcpy(char *dst, const char *src, int max) {
    int i = 0;
    while (src && src[i] && i < max - 1) { dst[i] = src[i]; i++; }
    dst[i] = 0;
}

/* ── draw text (scale=1 → 5×7 px per char, gap=1) ── */
static void draw_char(uint32_t x, uint32_t y, char c, uint32_t col) {
    if (c < 32 || c > 126) return;
    const uint8_t *bmp = font5x7[c - 32];
    for (int col_i = 0; col_i < 5; col_i++) {
        for (int row = 0; row < 7; row++) {
            if (bmp[col_i] & (1 << row))
                draw_pixel(x + col_i, y + row, col);
        }
    }
}

static void draw_text(uint32_t x, uint32_t y, const char *s, uint32_t col) {
    if (!s) return;
    uint32_t cx = x;
    for (int i = 0; s[i]; i++) {
        draw_char(cx, y, s[i], col);
        cx += 6; /* 5px + 1px gap */
    }
}

/* ── text width helper ── */
static uint32_t text_width(const char *s) {
    int n = gtk_strlen(s);
    return (uint32_t)(n * 6);
}

/* ── widget pool ── */
static GtkWidget  widget_pool[GTK_MAX_WIDGETS];
static int        widget_used = 0;
static GtkWindow  window_pool[4];
static int        window_used = 0;

static GtkWidget *alloc_widget(GtkWidgetType t) {
    if (widget_used >= GTK_MAX_WIDGETS) return NULL;
    GtkWidget *w = &widget_pool[widget_used++];
    w->type       = t;
    w->visible    = 1;
    w->active     = 0;
    w->focused    = 0;
    w->progress   = 0;
    w->child_count = 0;
    w->on_clicked = NULL;
    w->cb_data    = NULL;
    w->fg         = COL_TEXT;
    w->bg         = COL_BG;
    w->label[0]   = 0;
    return w;
}

/* ── API ── */
void gtk_init(int *argc, char ***argv) {
    (void)argc; (void)argv;
    widget_used = 0;
    window_used = 0;
}

GtkWindow *gtk_window_new(GtkWindowType type) {
    if (window_used >= 4) return NULL;
    GtkWindow *win = &window_pool[window_used++];
    win->x = 50; win->y = 50;
    win->w = 640; win->h = 480;
    win->visible = 1;
    win->root = NULL;
    gtk_strcpy(win->title, "Window", GTK_LABEL_LEN);
    (void)type;
    return win;
}

void gtk_window_set_title(GtkWindow *win, const char *title) {
    if (win) gtk_strcpy(win->title, title, GTK_LABEL_LEN);
}

void gtk_window_set_default_size(GtkWindow *win, int w, int h) {
    if (win) { win->w = (uint32_t)w; win->h = (uint32_t)h; }
}

void gtk_window_show_all(GtkWindow *win) {
    if (win) win->visible = 1;
}

void gtk_container_add(GtkWidget *container, GtkWidget *child) {
    if (!container || !child) return;
    if (container->child_count < GTK_MAX_CHILDREN)
        container->children[container->child_count++] = child;
}

GtkWidget *gtk_box_new(GtkOrientation o, int spacing) {
    GtkWidget *w = alloc_widget(GTK_WTYPE_BOX);
    if (!w) return NULL;
    w->orientation = o;
    w->spacing = spacing;
    w->bg = COL_WINDOW_BG;
    return w;
}

void gtk_box_pack_start(GtkWidget *box, GtkWidget *child,
                        int expand, int fill, int padding) {
    (void)expand; (void)fill; (void)padding;
    gtk_container_add(box, child);
}

GtkWidget *gtk_button_new_with_label(const char *label) {
    GtkWidget *w = alloc_widget(GTK_WTYPE_BUTTON);
    if (!w) return NULL;
    gtk_strcpy(w->label, label, GTK_LABEL_LEN);
    w->w = 120; w->h = 28;
    w->bg = COL_BUTTON_BG;
    return w;
}

void gtk_button_set_label(GtkWidget *btn, const char *label) {
    if (btn) gtk_strcpy(btn->label, label, GTK_LABEL_LEN);
}

GtkWidget *gtk_label_new(const char *text) {
    GtkWidget *w = alloc_widget(GTK_WTYPE_LABEL);
    if (!w) return NULL;
    gtk_strcpy(w->label, text, GTK_LABEL_LEN);
    w->h = 16;
    return w;
}

void gtk_label_set_text(GtkWidget *lbl, const char *text) {
    if (lbl) gtk_strcpy(lbl->label, text, GTK_LABEL_LEN);
}

GtkWidget *gtk_entry_new(void) {
    GtkWidget *w = alloc_widget(GTK_WTYPE_ENTRY);
    if (!w) return NULL;
    w->w = 200; w->h = 24;
    w->bg = COL_ENTRY_BG;
    return w;
}

void gtk_entry_set_text(GtkWidget *entry, const char *text) {
    if (entry) gtk_strcpy(entry->label, text, GTK_LABEL_LEN);
}

const char *gtk_entry_get_text(GtkWidget *entry) {
    return entry ? entry->label : "";
}

GtkWidget *gtk_check_button_new_with_label(const char *label) {
    GtkWidget *w = alloc_widget(GTK_WTYPE_CHECK);
    if (!w) return NULL;
    gtk_strcpy(w->label, label, GTK_LABEL_LEN);
    w->w = 160; w->h = 20;
    return w;
}

void gtk_toggle_button_set_active(GtkWidget *btn, int active) {
    if (btn) btn->active = (uint8_t)active;
}

int gtk_toggle_button_get_active(GtkWidget *btn) {
    return btn ? btn->active : 0;
}

GtkWidget *gtk_progress_bar_new(void) {
    GtkWidget *w = alloc_widget(GTK_WTYPE_PROGRESS);
    if (!w) return NULL;
    w->w = 200; w->h = 20;
    return w;
}

void gtk_progress_bar_set_fraction(GtkWidget *pb, uint32_t percent) {
    if (pb) pb->progress = percent > 100 ? 100 : percent;
}

GtkWidget *gtk_separator_new(GtkOrientation o) {
    GtkWidget *w = alloc_widget(GTK_WTYPE_SEPARATOR);
    if (!w) return NULL;
    w->orientation = o;
    w->h = 2; w->w = 200;
    return w;
}

void gtk_signal_connect(GtkWidget *w, const char *sig,
                        GCallback cb, void *data) {
    (void)sig;
    if (w) { w->on_clicked = cb; w->cb_data = data; }
}

/* ── render single widget at absolute (ax, ay) ── */
static void render_widget(GtkWidget *w, uint32_t ax, uint32_t ay);

static void render_box(GtkWidget *box, uint32_t ax, uint32_t ay) {
    draw_rect(ax, ay, box->w, box->h, box->bg);
    uint32_t cx = ax + 8, cy = ay + 8;
    for (int i = 0; i < box->child_count; i++) {
        GtkWidget *c = box->children[i];
        if (!c || !c->visible) continue;
        c->x = cx; c->y = cy;
        render_widget(c, cx, cy);
        if (box->orientation == GTK_ORIENTATION_VERTICAL)
            cy += c->h + (uint32_t)box->spacing + 4;
        else
            cx += c->w + (uint32_t)box->spacing + 4;
    }
}

static void render_widget(GtkWidget *w, uint32_t ax, uint32_t ay) {
    if (!w || !w->visible) return;

    switch (w->type) {

    case GTK_WTYPE_BOX:
        render_box(w, ax, ay);
        break;

    case GTK_WTYPE_BUTTON: {
        draw_rect(ax, ay, w->w, w->h, COL_BUTTON_BG);
        draw_border(ax, ay, w->w, w->h, COL_BUTTON_BRD);
        /* highlight top/left */
        draw_rect(ax+1, ay+1, w->w-2, 1, 0xFFFFFF);
        draw_rect(ax+1, ay+1, 1, w->h-2, 0xFFFFFF);
        uint32_t tw = text_width(w->label);
        uint32_t tx = ax + (w->w > tw ? (w->w - tw) / 2 : 2);
        uint32_t ty = ay + (w->h > 7 ? (w->h - 7) / 2 : 1);
        draw_text(tx, ty, w->label, COL_TEXT);
        break;
    }

    case GTK_WTYPE_LABEL:
        draw_text(ax, ay + 4, w->label, COL_TEXT);
        break;

    case GTK_WTYPE_ENTRY:
        draw_rect(ax, ay, w->w, w->h, COL_ENTRY_BG);
        draw_border(ax, ay, w->w, w->h, COL_ENTRY_BRD);
        draw_text(ax + 4, ay + (w->h > 7 ? (w->h - 7) / 2 : 1),
                  w->label, COL_TEXT);
        break;

    case GTK_WTYPE_CHECK: {
        /* box 14×14 */
        draw_rect(ax, ay + 3, 14, 14, COL_CHECK_BG);
        draw_border(ax, ay + 3, 14, 14, COL_CHECK_BRD);
        if (w->active) {
            /* draw tick */
            draw_rect(ax+3, ay+8,  2, 5, COL_CHECK_TICK);
            draw_rect(ax+5, ay+10, 6, 2, COL_CHECK_TICK);
            draw_rect(ax+9, ay+5,  2, 5, COL_CHECK_TICK);
        }
        draw_text(ax + 18, ay + 4, w->label, COL_TEXT);
        break;
    }

    case GTK_WTYPE_PROGRESS: {
        draw_rect(ax, ay, w->w, w->h, COL_PROGRESS_BG);
        draw_border(ax, ay, w->w, w->h, COL_BUTTON_BRD);
        uint32_t filled = (w->w - 2) * w->progress / 100;
        if (filled > 0)
            draw_rect(ax+1, ay+1, filled, w->h-2, COL_PROGRESS_FG);
        /* percentage text centred */
        char pct[5];
        uint32_t p = w->progress;
        pct[0] = '0' + (char)(p / 100);
        pct[1] = '0' + (char)((p / 10) % 10);
        pct[2] = '0' + (char)(p % 10);
        pct[3] = '%'; pct[4] = 0;
        uint32_t tw = text_width(pct);
        draw_text(ax + (w->w > tw ? (w->w - tw)/2 : 0),
                  ay + (w->h > 7 ? (w->h - 7)/2 : 0), pct, COL_TEXT);
        break;
    }

    case GTK_WTYPE_SEPARATOR:
        if (w->orientation == GTK_ORIENTATION_HORIZONTAL)
            draw_rect(ax, ay + w->h/2, w->w, 1, COL_SEP);
        else
            draw_rect(ax + w->w/2, ay, 1, w->h, COL_SEP);
        break;

    case GTK_WTYPE_IMAGE:
        draw_rect(ax, ay, w->w, w->h, 0xCCCCCC);
        draw_border(ax, ay, w->w, w->h, COL_BUTTON_BRD);
        break;
    }
}

/* ── render full window ── */
void gtk_render_window(GtkWindow *win) {
    if (!win || !win->visible) return;

    uint32_t x = win->x, y = win->y, w = win->w, h = win->h;

    /* shadow */
    draw_rect(x+4, y+4, w, h, 0xAAAAAA);

    /* window background */
    draw_rect(x, y, w, h, COL_WINDOW_BG);
    draw_border(x, y, w, h, 0x666666);

    /* title bar */
    draw_rect(x, y, w, 28, COL_TITLEBAR);
    /* title bar gradient (manual 4-step) */
    draw_rect(x, y,   w, 7, 0x5080C8);
    draw_rect(x, y+7, w, 7, 0x3C6EB4);
    draw_rect(x, y+14,w, 7, 0x2A5A9E);
    draw_rect(x, y+21,w, 7, 0x1E4A88);

    /* title text */
    draw_text(x + 8, y + 10, win->title, COL_TITLEBAR_TXT);

    /* close button */
    draw_rect(x + w - 22, y + 4, 18, 20, 0xCC3333);
    draw_border(x + w - 22, y + 4, 18, 20, 0x991111);
    draw_text(x + w - 17, y + 10, "X", COL_TITLEBAR_TXT);

    /* client area */
    draw_rect(x+1, y+28, w-2, h-29, COL_WINDOW_BG);

    /* root widget fills client area */
    if (win->root) {
        win->root->x = x + 1;
        win->root->y = y + 28;
        win->root->w = w - 2;
        win->root->h = h - 29;
        render_widget(win->root, win->root->x, win->root->y);
    }
}

/* ── mouse event queue ── */
static GtkMouseEvent ev_queue[GTK_EVENT_QUEUE_SIZE];
static int ev_head = 0, ev_tail = 0;

void gtk_push_mouse_event(GtkMouseEvent *ev) {
    int next = (ev_tail + 1) % GTK_EVENT_QUEUE_SIZE;
    if (next == ev_head) return;
    ev_queue[ev_tail] = *ev;
    ev_tail = next;
}

int gtk_pop_mouse_event(GtkMouseEvent *ev) {
    if (ev_head == ev_tail) return 0;
    *ev = ev_queue[ev_head];
    ev_head = (ev_head + 1) % GTK_EVENT_QUEUE_SIZE;
    return 1;
}

/* ── keyboard event queue ── */
static GtkKeyEvent key_queue[GTK_EVENT_QUEUE_SIZE];
static int key_head = 0, key_tail = 0;

void gtk_push_key_event(GtkKeyEvent *ev) {
    int next = (key_tail + 1) % GTK_EVENT_QUEUE_SIZE;
    if (next == key_head) return;
    key_queue[key_tail] = *ev;
    key_tail = next;
}

int gtk_pop_key_event(GtkKeyEvent *ev) {
    if (key_head == key_tail) return 0;
    *ev = key_queue[key_head];
    key_head = (key_head + 1) % GTK_EVENT_QUEUE_SIZE;
    return 1;
}

void gtk_poll_events(GtkWindow *win) {
    static uint8_t prev_btn = 0;
    GtkMouseEvent mev;
    GtkKeyEvent kev;
    
    /* mouse events */
    while (gtk_pop_mouse_event(&mev)) {
        if ((mev.buttons & 1) && !(prev_btn & 1))
            gtk_handle_click(win, mev.x, mev.y);
        prev_btn = mev.buttons;
    }
    
    /* keyboard events */
    while (gtk_pop_key_event(&kev)) {
        if (kev.pressed && kev.ascii) {
            /* find focused entry widget */
            /* TODO: implement focus system */
        }
    }
}

static void dispatch_click(GtkWidget *w, int32_t mx, int32_t my) {
    if (!w || !w->visible) return;
    int32_t wx = (int32_t)w->x, wy = (int32_t)w->y;
    int32_t ww = (int32_t)w->w, wh = (int32_t)w->h;
    if (mx < wx || mx >= wx+ww || my < wy || my >= wy+wh) return;

    if (w->type == GTK_WTYPE_BUTTON && w->on_clicked)
        w->on_clicked(w, w->cb_data);
    if (w->type == GTK_WTYPE_CHECK)
        w->active = !w->active;

    for (int i = 0; i < w->child_count; i++)
        dispatch_click(w->children[i], mx, my);
}

void gtk_handle_click(GtkWindow *win, int32_t mx, int32_t my) {
    if (!win || !win->visible || !win->root) return;
    dispatch_click(win->root, mx, my);
}
