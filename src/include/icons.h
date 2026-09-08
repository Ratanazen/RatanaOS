#ifndef ICONS_H
#define ICONS_H

#include "types.h"

// Vector / Procedural Icon Renderers for macOS Theme
void icon_draw_apple(int x, int y, uint32_t color);
void icon_draw_finder(int x, int y, int size);
void icon_draw_terminal(int x, int y, int size);
void icon_draw_sysmon(int x, int y, int size);
void icon_draw_calculator(int x, int y, int size);
void icon_draw_paint(int x, int y, int size);
void icon_draw_about(int x, int y, int size);
void icon_draw_trash(int x, int y, int size);
void icon_draw_drive(int x, int y);
void icon_draw_wifi(int x, int y, uint32_t color);
void icon_draw_battery(int x, int y, uint32_t color);
void icon_draw_search(int x, int y, uint32_t color);
void icon_draw_control_center(int x, int y, uint32_t color);

#endif // ICONS_H
