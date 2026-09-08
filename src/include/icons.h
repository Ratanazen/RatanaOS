#ifndef ICONS_H
#define ICONS_H

#include "types.h"

// 48x48 macOS Sonoma/Sequoia Dock Icons
void icon_draw_finder_48(int x, int y);
void icon_draw_launchpad_48(int x, int y);
void icon_draw_safari_48(int x, int y);
void icon_draw_terminal_48(int x, int y);
void icon_draw_sysmon_48(int x, int y);
void icon_draw_calculator_48(int x, int y);
void icon_draw_paint_48(int x, int y);
void icon_draw_notes_48(int x, int y);
void icon_draw_music_48(int x, int y);
void icon_draw_about_48(int x, int y);
void icon_draw_appstore_48(int x, int y);
void icon_draw_settings_48(int x, int y);
void icon_draw_trash_48(int x, int y);

// Desktop Icons (48x48)
void icon_draw_drive_48(int x, int y);
void icon_draw_folder_48(int x, int y);

// 64x64 Large Apple Emblem for "About This Mac" Dialog
void icon_draw_apple_64(int x, int y);

// Menu Bar Icons & Status Extras (16x16 / 24x24)
void icon_draw_apple_logo(int x, int y, uint32_t color);
void icon_draw_wifi(int x, int y, uint32_t color);
void icon_draw_battery(int x, int y, uint32_t color);
void icon_draw_search(int x, int y, uint32_t color);
void icon_draw_control_center(int x, int y, uint32_t color);
void icon_draw_siri(int x, int y);

#endif // ICONS_H
