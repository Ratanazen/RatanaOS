#ifndef DOCK_H
#define DOCK_H

#include "types.h"

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

void dock_set_icon_size(int size);
int  dock_get_icon_size(void);
void dock_set_spacing(int spacing);
int  dock_get_spacing(void);
void dock_set_magnification(bool enabled);
bool dock_get_magnification(void);

#endif // DOCK_H
