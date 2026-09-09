#ifndef ICONS_H
#define ICONS_H

#include "types.h"
#include "icons_assets.h"

// Theme & Configuration Management API
typedef struct {
    icon_theme_id_t theme;              // ICON_THEME_WHITESUR, ICON_THEME_MACTAHOE, ICON_THEME_VECTOR
    int             dock_icon_size;     // 32, 40, 48, 56, 64 px (default 48)
    int             dock_spacing;       // 2 to 16 px (default 6)
    bool            dock_magnification; // true / false (default true)
    bool            show_desktop_icons; // true / false (default true)
    int             desktop_icon_size;  // 32, 48, 64 px (default 48)
    bool            show_icon_labels;   // true / false (default true)
} icon_config_t;

void icon_config_init(void);
void icon_config_get(icon_config_t* cfg);
void icon_config_set(const icon_config_t* cfg);
void icon_config_reset_defaults(void);
void icon_config_apply(void);

void icon_set_theme(icon_theme_id_t theme);
icon_theme_id_t icon_get_theme(void);
void icon_theme_next(void);
const char* icon_get_theme_name(void);

// 32-bit ARGB Icon Blitter with 8-bit Alpha Channel
void gfx_draw_icon_rgba(int x, int y, int w, int h, const uint32_t* pixels);
void gfx_draw_icon_rgba_scaled(int x, int y, int target_w, int target_h, int src_w, int src_h, const uint32_t* pixels);

// Scaled Icon Drawing (Supports Theme Assets & Vector fallback)
void icon_draw_scaled(icon_id_t id, int x, int y, int size);

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
