#ifndef MOUSE_H
#define MOUSE_H

#include "types.h"

void mouse_init(void);
int  mouse_get_x(void);
int  mouse_get_y(void);
bool mouse_is_left_clicked(void);
bool mouse_is_right_clicked(void);
void mouse_set_bounds(int max_x, int max_y);
void mouse_draw_cursor(int x, int y);

#endif // MOUSE_H
