#ifndef _GTK_H
#define _GTK_H

#include <stdint.h>

/* ── limits ── */
#define GTK_MAX_WIDGETS      48
#define GTK_MAX_CHILDREN     16
#define GTK_LABEL_LEN        64
#define GTK_EVENT_QUEUE_SIZE 64

/* ── enums ── */
typedef enum { GTK_WINDOW_TOPLEVEL, GTK_WINDOW_POPUP } GtkWindowType;
typedef enum { GTK_ORIENTATION_HORIZONTAL, GTK_ORIENTATION_VERTICAL } GtkOrientation;
typedef enum {
    GTK_WTYPE_BOX,
    GTK_WTYPE_BUTTON,
    GTK_WTYPE_LABEL,
    GTK_WTYPE_ENTRY,
    GTK_WTYPE_CHECK,
    GTK_WTYPE_PROGRESS,
    GTK_WTYPE_SEPARATOR,
    GTK_WTYPE_IMAGE
} GtkWidgetType;

typedef void (*GCallback)(void *widget, void *data);

/* ── widget ── */
typedef struct GtkWidget {
    GtkWidgetType    type;
    uint32_t         x, y, w, h;
    char             label[GTK_LABEL_LEN];
    uint8_t          visible;
    uint8_t          active;    /* check/toggle */
    uint8_t          focused;   /* keyboard focus */
    uint8_t          hovered;   /* mouse hover */
    uint32_t         progress;  /* 0-100 */
    GtkOrientation   orientation;
    int              spacing;
    GCallback        on_clicked;
    void            *cb_data;
    struct GtkWidget *children[GTK_MAX_CHILDREN];
    int              child_count;
    uint32_t         fg, bg;
} GtkWidget;

typedef struct {
    uint32_t   x, y, w, h;
    char       title[GTK_LABEL_LEN];
    uint8_t    visible;
    GtkWidget *root;
    /* drag state */
    uint8_t    dragging;
    int32_t    drag_ox, drag_oy;  /* offset from window origin to click point */
} GtkWindow;

/* ── mouse event ── */
typedef struct {
    int32_t  x, y;
    int8_t   dx, dy;
    uint8_t  buttons;
} GtkMouseEvent;

/* ── keyboard event ── */
typedef struct {
    uint8_t scancode;
    char    ascii;
    uint8_t pressed;
} GtkKeyEvent;

/* ── init ── */
void gtk_init(int *argc, char ***argv);

/* ── window ── */
GtkWindow  *gtk_window_new(GtkWindowType type);
void        gtk_window_set_title(GtkWindow *win, const char *title);
void        gtk_window_set_default_size(GtkWindow *win, int w, int h);
void        gtk_window_show_all(GtkWindow *win);

/* ── container ── */
void        gtk_container_add(GtkWidget *container, GtkWidget *child);

/* ── box ── */
GtkWidget  *gtk_box_new(GtkOrientation o, int spacing);
void        gtk_box_pack_start(GtkWidget *box, GtkWidget *child,
                               int expand, int fill, int padding);

/* ── button ── */
GtkWidget  *gtk_button_new_with_label(const char *label);
void        gtk_button_set_label(GtkWidget *btn, const char *label);

/* ── label ── */
GtkWidget  *gtk_label_new(const char *text);
void        gtk_label_set_text(GtkWidget *lbl, const char *text);

/* ── entry ── */
GtkWidget  *gtk_entry_new(void);
void        gtk_entry_set_text(GtkWidget *entry, const char *text);
const char *gtk_entry_get_text(GtkWidget *entry);

/* ── check button ── */
GtkWidget  *gtk_check_button_new_with_label(const char *label);
void        gtk_toggle_button_set_active(GtkWidget *btn, int active);
int         gtk_toggle_button_get_active(GtkWidget *btn);

/* ── progress bar ── */
GtkWidget  *gtk_progress_bar_new(void);
void        gtk_progress_bar_set_fraction(GtkWidget *pb, uint32_t percent);

/* ── separator ── */
GtkWidget  *gtk_separator_new(GtkOrientation o);

/* ── signals ── */
void gtk_signal_connect(GtkWidget *w, const char *sig,
                        GCallback cb, void *data);

/* ── event queue ── */
void gtk_push_mouse_event(GtkMouseEvent *ev);
int  gtk_pop_mouse_event(GtkMouseEvent *ev);
void gtk_push_key_event(GtkKeyEvent *ev);
int  gtk_pop_key_event(GtkKeyEvent *ev);

/* ── poll (call once per frame) ── */
void gtk_poll_events(GtkWindow *win);

/* ── render ── */
void gtk_render_window(GtkWindow *win);

/* ── internal ── */
void gtk_handle_click(GtkWindow *win, int32_t mx, int32_t my);

#endif
