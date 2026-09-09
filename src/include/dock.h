#ifndef DOCK_H
#define DOCK_H

#include "types.h"
#include "theme.h"

#define DOCK_NUM_ITEMS 13
#define DOCK_ICON_SIZE 48
#define DOCK_PADDING 10
#define DOCK_SPACING 6
#define DOCK_RADIUS 16

typedef struct {
    int id;
    const char* name;
    bool is_open;
    bool is_active;
} dock_item_t;

void dock_init(void);
void dock_draw(const bool* window_open_states, const bool* window_active_states, int count);
int  dock_hit_test(int mx, int my);
void dock_update_cursor(int mx, int my);

void dock_set_icon_size(int size);
int  dock_get_icon_size(void);
void dock_set_spacing(int spacing);
int  dock_get_spacing(void);
void dock_set_magnification(bool enabled);
bool dock_get_magnification(void);

void dock_set_position(dock_position_t pos);
dock_position_t dock_get_position(void);
void dock_set_style(dock_style_t style);
dock_style_t dock_get_style(void);
void dock_set_autohide(bool enabled);
bool dock_get_autohide(void);
bool dock_is_visible(void);

#endif // DOCK_H
