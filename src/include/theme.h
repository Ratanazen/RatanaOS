#ifndef THEME_H
#define THEME_H

#include "types.h"

typedef enum {
    UI_THEME_RATANA_DARK = 0,
    UI_THEME_RATANA_LIGHT,
    UI_THEME_MACOS_DARK,
    UI_THEME_MACOS_LIGHT,
    UI_THEME_AUTO,
    UI_THEME_COUNT
} ui_theme_preset_t;

typedef enum {
    ACCENT_BLUE = 0,
    ACCENT_PURPLE,
    ACCENT_PINK,
    ACCENT_RED,
    ACCENT_ORANGE,
    ACCENT_YELLOW,
    ACCENT_GREEN,
    ACCENT_GRAPHITE,
    ACCENT_COUNT
} ui_accent_color_t;

typedef struct {
    // Preset ID & State
    ui_theme_preset_t preset;
    ui_accent_color_t accent_id;
    bool dark_mode;
    bool transparency_enabled;
    bool shadows_enabled;

    // Desktop & Wallpaper Colors
    uint32_t wallpaper_top;
    uint32_t wallpaper_mid;
    uint32_t wallpaper_bot;

    // Window Colors & Chrome
    uint32_t window_bg;
    uint32_t window_titlebar;
    uint32_t window_border_active;
    uint32_t window_border_inactive;
    uint32_t window_title_active;
    uint32_t window_title_inactive;

    // Sidebars & Panels
    uint32_t sidebar_bg;
    uint32_t sidebar_border;
    uint32_t sidebar_item_hover;
    uint32_t sidebar_item_active;
    uint32_t panel_bg;
    uint32_t panel_border;

    // Typography Colors
    uint32_t text_primary;
    uint32_t text_secondary;
    uint32_t text_disabled;
    uint32_t text_on_accent;

    // Accent, Borders & Separators
    uint32_t accent;
    uint32_t border;
    uint32_t separator;

    // Interactive Controls & Buttons
    uint32_t button_bg;
    uint32_t button_border;
    uint32_t button_hover;
    uint32_t button_pressed;
    uint32_t button_text;

    uint32_t control_bg;
    uint32_t control_border;
    uint32_t control_active;

    uint32_t selection_bg;
    uint32_t selection_text;

    // Alpha / Translucency
    uint8_t window_alpha;
    uint8_t menubar_alpha;
    uint8_t dock_alpha;
    uint8_t panel_alpha;
    uint8_t shadow_alpha;

    // Metrics & Radii
    int window_radius;
    int window_shadow_size;
    int button_radius;
    int dock_radius;
    int control_radius;
    int sidebar_radius;
} ui_theme_t;

// UI Scale Helper Macro
#define UI(x) ui_scale_val(x)

void theme_init(void);
const ui_theme_t* theme_get_current(void);
void theme_set_preset(ui_theme_preset_t preset);
ui_theme_preset_t theme_get_preset(void);
const char* theme_get_preset_name(ui_theme_preset_t preset);

void theme_set_accent(ui_accent_color_t accent);
ui_accent_color_t theme_get_accent(void);
const char* theme_get_accent_name(ui_accent_color_t accent);
uint32_t theme_get_accent_color(ui_accent_color_t accent);

void theme_toggle_dark(void);
void theme_next(void);
void theme_set_transparency(bool enabled);
void theme_set_shadows(bool enabled);
void theme_set_window_radius(int radius);
void theme_update(void); // Updates auto mode from RTC

// Global UI Scaling
void ui_scale_set(int percent);
int  ui_scale_get(void);
void ui_scale_increase(void);
void ui_scale_decrease(void);
int  ui_scale_val(int val);
const char* ui_scale_get_label(void);

/* Stable GUI-facing aliases. Icon selection deliberately remains separate. */
void ui_theme_set(ui_theme_preset_t preset);
const ui_theme_t* ui_theme_get(void);
void ui_theme_next(void);
const char* ui_theme_get_name(void);
void ui_scale_up(void);
void ui_scale_down(void);
int ui_scale_value(void);

#endif // THEME_H
