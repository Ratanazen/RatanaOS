#ifndef GUI_H
#define GUI_H

#include "types.h"
#include "multiboot.h"

#define MAX_WINDOWS 16

typedef enum {
    TITLEBAR_HIT_NONE = 0,
    TITLEBAR_HIT_CLOSE,
    TITLEBAR_HIT_MINIMIZE,
    TITLEBAR_HIT_ZOOM,
    TITLEBAR_HIT_DRAG
} titlebar_hit_t;

typedef struct window {
    int id;
    char title[32];
    int x;
    int y;
    int width;
    int height;
    int orig_w;
    int orig_h;
    bool is_open;
    bool is_minimized;
    bool is_active;
    bool is_dragging;
    int drag_offset_x;
    int drag_offset_y;

    void (*draw_content)(struct window* win);
    void (*handle_click)(struct window* win, int rel_x, int rel_y, int btn);
} window_t;

void gui_init(multiboot_info_t* mbi);
void gui_start(void);
void gui_exit(void);
bool gui_is_running(void);
bool gui_launch_app(const char* app_id);

#endif // GUI_H
