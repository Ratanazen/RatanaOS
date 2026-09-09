#include "../include/theme.h"
#include "../include/gfx.h"
#include "../include/rtc.h"
#include "../include/string.h"
#include "../include/font.h"

static ui_theme_t current_theme;
static int global_ui_scale_percent = 100; // 100% default

static const uint32_t accent_colors[ACCENT_COUNT] = {
    0x000A84FF, // Blue
    0x00BF5AF2, // Purple
    0x00FF375F, // Pink
    0x00FF453A, // Red
    0x00FF9F0A, // Orange
    0x00FFD60A, // Yellow
    0x0030D158, // Green
    0x008E8E93  // Graphite
};

static const char* accent_names[ACCENT_COUNT] = {
    "Blue", "Purple", "Pink", "Red",
    "Orange", "Yellow", "Green", "Graphite"
};

static const char* preset_names[UI_THEME_COUNT] = {
    "RatanaOS Dark",
    "RatanaOS Light",
    "macOS Dark",
    "macOS Light",
    "Auto (Day/Night)"
};

static const char* window_style_names[WINDOW_STYLE_COUNT] = {
    "macOS Standard",
    "Classic Rect",
    "Minimal Flat",
    "Transparent Acrylic"
};

static const char* dock_position_names[DOCK_POS_COUNT] = {
    "Bottom",
    "Left",
    "Right"
};

static const char* dock_style_names[DOCK_STYLE_COUNT] = {
    "Frosted Glass",
    "Classic Solid",
    "Transparent",
    "Compact"
};

static const char* menubar_style_names[MENUBAR_STYLE_COUNT] = {
    "Frosted Glass",
    "Solid Opaque",
    "Transparent"
};

static const char* theme_mode_names[THEME_MODE_COUNT] = {
    "Light",
    "Dark",
    "Auto (Day/Night)"
};

static const int ui_scale_steps[] = { 80, 90, 100, 110, 125, 150, 175, 200 };
#define NUM_SCALE_STEPS (int)(sizeof(ui_scale_steps) / sizeof(ui_scale_steps[0]))

static void apply_theme_preset(ui_theme_preset_t preset, ui_accent_color_t accent) {
    current_theme.preset = preset;
    current_theme.accent_id = accent;
    uint32_t acc = accent_colors[accent];
    current_theme.accent = acc;

    // Resolve Auto mode
    bool is_dark = true;
    if (preset == UI_THEME_AUTO) {
        rtc_time_t t = rtc_get_time();
        if (t.hour >= 6 && t.hour < 18) {
            is_dark = false;
        } else {
            is_dark = true;
        }
    } else if (preset == UI_THEME_RATANA_LIGHT || preset == UI_THEME_MACOS_LIGHT) {
        is_dark = false;
    }

    current_theme.dark_mode = is_dark;
    current_theme.transparency_enabled = true;
    current_theme.shadows_enabled = true;
    current_theme.window_radius = 10;
    current_theme.window_border_width = 1;
    current_theme.window_shadow_size = 6;
    current_theme.button_radius = 6;
    current_theme.dock_radius = 16;
    current_theme.control_radius = 9;
    current_theme.sidebar_radius = 6;
    current_theme.window_style = WINDOW_STYLE_MACOS;
    current_theme.dock_position = DOCK_POS_BOTTOM;
    current_theme.dock_style = DOCK_STYLE_GLASS;
    current_theme.menubar_style = MENUBAR_STYLE_GLASS;
    current_theme.dock_autohide = false;
    if (current_theme.font_scale < 80 || current_theme.font_scale > 150) {
        current_theme.font_scale = 100;
    }

    if (is_dark) {
        if (preset == UI_THEME_RATANA_DARK || preset == UI_THEME_AUTO) {
            // RatanaOS Dark - Midnight Blue & Obsidian
            current_theme.wallpaper_top = 0x001C1C2E;
            current_theme.wallpaper_mid = 0x002E2E48;
            current_theme.wallpaper_bot = 0x00141420;

            current_theme.window_bg = 0x001E1E24;
            current_theme.window_titlebar = 0x002B2B2E;
            current_theme.window_border_active = 0x0050505A;
            current_theme.window_border_inactive = 0x0032323A;
            current_theme.window_title_active = 0x00FFFFFF;
            current_theme.window_title_inactive = 0x008E8E93;

            current_theme.sidebar_bg = 0x00242428;
            current_theme.sidebar_border = 0x003A3A3C;
            current_theme.sidebar_item_hover = 0x00323238;
            current_theme.sidebar_item_active = acc;
            current_theme.panel_bg = 0x001C1C1E;
            current_theme.panel_border = 0x003A3A3C;

            current_theme.text_primary = 0x00FFFFFF;
            current_theme.text_secondary = 0x008E8E93;
            current_theme.text_disabled = 0x00636366;
            current_theme.text_on_accent = 0x00FFFFFF;

            current_theme.border = 0x003A3A3C;
            current_theme.separator = 0x003A3A3C;

            current_theme.button_bg = 0x002C2C2E;
            current_theme.button_border = 0x0048484A;
            current_theme.button_hover = 0x003A3A3C;
            current_theme.button_pressed = 0x001C1C1E;
            current_theme.button_text = 0x00FFFFFF;

            current_theme.control_bg = 0x003A3A3C;
            current_theme.control_border = 0x00545458;
            current_theme.control_active = 0x0030D158;

            current_theme.selection_bg = acc;
            current_theme.selection_text = 0x00FFFFFF;

            current_theme.window_alpha = 245;
            current_theme.menubar_alpha = 230;
            current_theme.dock_alpha = 215;
            current_theme.panel_alpha = 235;
            current_theme.shadow_alpha = 160;
        } else {
            // macOS Dark - Space Charcoal
            current_theme.wallpaper_top = 0x00151518;
            current_theme.wallpaper_mid = 0x0023232A;
            current_theme.wallpaper_bot = 0x00101012;

            current_theme.window_bg = 0x00202022;
            current_theme.window_titlebar = 0x002D2D30;
            current_theme.window_border_active = 0x00606068;
            current_theme.window_border_inactive = 0x00383840;
            current_theme.window_title_active = 0x00FFFFFF;
            current_theme.window_title_inactive = 0x0098989E;

            current_theme.sidebar_bg = 0x0026262A;
            current_theme.sidebar_border = 0x00404046;
            current_theme.sidebar_item_hover = 0x0036363C;
            current_theme.sidebar_item_active = acc;
            current_theme.panel_bg = 0x001E1E20;
            current_theme.panel_border = 0x003E3E44;

            current_theme.text_primary = 0x00FFFFFF;
            current_theme.text_secondary = 0x00A0A0A8;
            current_theme.text_disabled = 0x00686870;
            current_theme.text_on_accent = 0x00FFFFFF;

            current_theme.border = 0x00404046;
            current_theme.separator = 0x00383840;

            current_theme.button_bg = 0x00303034;
            current_theme.button_border = 0x00505058;
            current_theme.button_hover = 0x00404048;
            current_theme.button_pressed = 0x00202024;
            current_theme.button_text = 0x00FFFFFF;

            current_theme.control_bg = 0x003E3E44;
            current_theme.control_border = 0x00585860;
            current_theme.control_active = 0x0030D158;

            current_theme.selection_bg = acc;
            current_theme.selection_text = 0x00FFFFFF;

            current_theme.window_alpha = 245;
            current_theme.menubar_alpha = 230;
            current_theme.dock_alpha = 215;
            current_theme.panel_alpha = 235;
            current_theme.shadow_alpha = 170;
        }
    } else {
        if (preset == UI_THEME_RATANA_LIGHT) {
            // RatanaOS Light - Crisp Platinum & Sky Gradient
            current_theme.wallpaper_top = 0x0068A0D8;
            current_theme.wallpaper_mid = 0x0088C0E8;
            current_theme.wallpaper_bot = 0x00A0D8F8;

            current_theme.window_bg = 0x00F0F0F4;
            current_theme.window_titlebar = 0x00E4E4E8;
            current_theme.window_border_active = 0x00B0B0B8;
            current_theme.window_border_inactive = 0x00D0D0D6;
            current_theme.window_title_active = 0x001A1A1E;
            current_theme.window_title_inactive = 0x00787880;

            current_theme.sidebar_bg = 0x00E6E6EA;
            current_theme.sidebar_border = 0x00D0D0D8;
            current_theme.sidebar_item_hover = 0x00D8D8E0;
            current_theme.sidebar_item_active = acc;
            current_theme.panel_bg = 0x00F8F8FA;
            current_theme.panel_border = 0x00D8D8E0;

            current_theme.text_primary = 0x001A1A1E;
            current_theme.text_secondary = 0x006C6C74;
            current_theme.text_disabled = 0x00A0A0A8;
            current_theme.text_on_accent = 0x00FFFFFF;

            current_theme.border = 0x00D0D0D8;
            current_theme.separator = 0x00DCDCE2;

            current_theme.button_bg = 0x00FFFFFF;
            current_theme.button_border = 0x00C8C8D0;
            current_theme.button_hover = 0x00EAEAEF;
            current_theme.button_pressed = 0x00D8D8DE;
            current_theme.button_text = 0x001A1A1E;

            current_theme.control_bg = 0x00D8D8DE;
            current_theme.control_border = 0x00B8B8C0;
            current_theme.control_active = 0x0030D158;

            current_theme.selection_bg = acc;
            current_theme.selection_text = 0x00FFFFFF;

            current_theme.window_alpha = 250;
            current_theme.menubar_alpha = 240;
            current_theme.dock_alpha = 220;
            current_theme.panel_alpha = 245;
            current_theme.shadow_alpha = 110;
        } else {
            // macOS Light - Silver Aqua
            current_theme.wallpaper_top = 0x007AB0E0;
            current_theme.wallpaper_mid = 0x009AC8F0;
            current_theme.wallpaper_bot = 0x00C0E0F8;

            current_theme.window_bg = 0x00F5F5F7;
            current_theme.window_titlebar = 0x00E8E8EC;
            current_theme.window_border_active = 0x00B8B8C2;
            current_theme.window_border_inactive = 0x00D4D4DC;
            current_theme.window_title_active = 0x00202024;
            current_theme.window_title_inactive = 0x00808088;

            current_theme.sidebar_bg = 0x00ECECF0;
            current_theme.sidebar_border = 0x00D4D4DC;
            current_theme.sidebar_item_hover = 0x00DFDFE6;
            current_theme.sidebar_item_active = acc;
            current_theme.panel_bg = 0x00FAFAFC;
            current_theme.panel_border = 0x00DCDCE4;

            current_theme.text_primary = 0x00202024;
            current_theme.text_secondary = 0x00707078;
            current_theme.text_disabled = 0x00A4A4AC;
            current_theme.text_on_accent = 0x00FFFFFF;

            current_theme.border = 0x00D4D4DC;
            current_theme.separator = 0x00E0E0E6;

            current_theme.button_bg = 0x00FFFFFF;
            current_theme.button_border = 0x00CCCCD4;
            current_theme.button_hover = 0x00EDEDF2;
            current_theme.button_pressed = 0x00DBDBE2;
            current_theme.button_text = 0x00202024;

            current_theme.control_bg = 0x00DCDCE2;
            current_theme.control_border = 0x00BCBCC4;
            current_theme.control_active = 0x0030D158;

            current_theme.selection_bg = acc;
            current_theme.selection_text = 0x00FFFFFF;

            current_theme.window_alpha = 250;
            current_theme.menubar_alpha = 240;
            current_theme.dock_alpha = 220;
            current_theme.panel_alpha = 245;
            current_theme.shadow_alpha = 110;
        }
    }

    // Assign semantic tokens
    current_theme.surface = current_theme.panel_bg;
    current_theme.surface_secondary = current_theme.sidebar_bg;
    current_theme.accent_hover = acc;
    current_theme.accent_pressed = acc;
    current_theme.menu_bar = current_theme.window_titlebar;
    current_theme.dock_bg = current_theme.panel_bg;
    current_theme.shadow = 0x00000000;

    // Refresh wallpaper buffer with active theme gradient
    gfx_generate_wallpaper(current_theme.wallpaper_top, current_theme.wallpaper_mid, current_theme.wallpaper_bot);
}

void theme_init(void) {
    global_ui_scale_percent = 100;
    apply_theme_preset(UI_THEME_RATANA_DARK, ACCENT_BLUE);
}

const ui_theme_t* theme_get_current(void) {
    return &current_theme;
}

void theme_set_preset(ui_theme_preset_t preset) {
    if (preset >= 0 && preset < UI_THEME_COUNT) {
        apply_theme_preset(preset, current_theme.accent_id);
    }
}

ui_theme_preset_t theme_get_preset(void) {
    return current_theme.preset;
}

const char* theme_get_preset_name(ui_theme_preset_t preset) {
    if (preset >= 0 && preset < UI_THEME_COUNT) {
        return preset_names[preset];
    }
    return "Unknown";
}

void theme_set_accent(ui_accent_color_t accent) {
    if (accent >= 0 && accent < ACCENT_COUNT) {
        apply_theme_preset(current_theme.preset, accent);
    }
}

ui_accent_color_t theme_get_accent(void) {
    return current_theme.accent_id;
}

const char* theme_get_accent_name(ui_accent_color_t accent) {
    if (accent >= 0 && accent < ACCENT_COUNT) {
        return accent_names[accent];
    }
    return "Blue";
}

uint32_t theme_get_accent_color(ui_accent_color_t accent) {
    if (accent >= 0 && accent < ACCENT_COUNT) {
        return accent_colors[accent];
    }
    return accent_colors[0];
}

void theme_toggle_dark(void) {
    if (current_theme.dark_mode) {
        theme_set_preset(UI_THEME_RATANA_LIGHT);
    } else {
        theme_set_preset(UI_THEME_RATANA_DARK);
    }
}

void theme_next(void) {
    ui_theme_preset_t next_p = (ui_theme_preset_t)((current_theme.preset + 1) % UI_THEME_COUNT);
    theme_set_preset(next_p);
}

void theme_set_transparency(bool enabled) {
    current_theme.transparency_enabled = enabled;
}

void theme_set_shadows(bool enabled) {
    current_theme.shadows_enabled = enabled;
}

void theme_set_window_radius(int radius) {
    if (radius >= 0 && radius <= 24) {
        current_theme.window_radius = radius;
    }
}

void theme_set_mode(theme_mode_t mode) {
    if (mode == THEME_MODE_LIGHT) {
        theme_set_preset(UI_THEME_RATANA_LIGHT);
    } else if (mode == THEME_MODE_DARK) {
        theme_set_preset(UI_THEME_RATANA_DARK);
    } else if (mode == THEME_MODE_AUTO) {
        theme_set_preset(UI_THEME_AUTO);
    }
    current_theme.mode = mode;
}

theme_mode_t theme_get_mode(void) {
    return current_theme.mode;
}

const char* theme_get_mode_name(theme_mode_t mode) {
    if (mode >= 0 && mode < THEME_MODE_COUNT) return theme_mode_names[mode];
    return "Dark";
}

void theme_set_window_style(window_style_t style) {
    if (style >= 0 && style < WINDOW_STYLE_COUNT) {
        current_theme.window_style = style;
        if (style == WINDOW_STYLE_CLASSIC) {
            current_theme.window_radius = 4;
            current_theme.window_shadow_size = 4;
        } else if (style == WINDOW_STYLE_MINIMAL) {
            current_theme.window_radius = 0;
            current_theme.window_shadow_size = 0;
            current_theme.shadows_enabled = false;
        } else if (style == WINDOW_STYLE_TRANSPARENT) {
            current_theme.window_radius = 12;
            current_theme.transparency_enabled = true;
            current_theme.window_alpha = 200;
        } else { // WINDOW_STYLE_MACOS
            current_theme.window_radius = 10;
            current_theme.window_shadow_size = 6;
            current_theme.shadows_enabled = true;
            current_theme.transparency_enabled = true;
            current_theme.window_alpha = 245;
        }
    }
}

window_style_t theme_get_window_style(void) {
    return current_theme.window_style;
}

const char* theme_get_window_style_name(window_style_t style) {
    if (style >= 0 && style < WINDOW_STYLE_COUNT) return window_style_names[style];
    return "macOS Standard";
}

void theme_set_window_shadow_size(int size) {
    if (size >= 0 && size <= 16) {
        current_theme.window_shadow_size = size;
        current_theme.shadows_enabled = (size > 0);
    }
}

void theme_set_dock_position(dock_position_t pos) {
    if (pos >= 0 && pos < DOCK_POS_COUNT) {
        current_theme.dock_position = pos;
    }
}

dock_position_t theme_get_dock_position(void) {
    return current_theme.dock_position;
}

const char* theme_get_dock_position_name(dock_position_t pos) {
    if (pos >= 0 && pos < DOCK_POS_COUNT) return dock_position_names[pos];
    return "Bottom";
}

void theme_set_dock_style(dock_style_t style) {
    if (style >= 0 && style < DOCK_STYLE_COUNT) {
        current_theme.dock_style = style;
        if (style == DOCK_STYLE_GLASS) {
            current_theme.dock_alpha = 215;
            current_theme.dock_radius = 16;
        } else if (style == DOCK_STYLE_CLASSIC) {
            current_theme.dock_alpha = 255;
            current_theme.dock_radius = 8;
        } else if (style == DOCK_STYLE_TRANSPARENT) {
            current_theme.dock_alpha = 140;
            current_theme.dock_radius = 16;
        } else if (style == DOCK_STYLE_COMPACT) {
            current_theme.dock_alpha = 230;
            current_theme.dock_radius = 10;
        }
    }
}

dock_style_t theme_get_dock_style(void) {
    return current_theme.dock_style;
}

const char* theme_get_dock_style_name(dock_style_t style) {
    if (style >= 0 && style < DOCK_STYLE_COUNT) return dock_style_names[style];
    return "Frosted Glass";
}

void theme_set_dock_autohide(bool enabled) {
    current_theme.dock_autohide = enabled;
}

bool theme_get_dock_autohide(void) {
    return current_theme.dock_autohide;
}

void theme_set_menubar_style(menubar_style_t style) {
    if (style >= 0 && style < MENUBAR_STYLE_COUNT) {
        current_theme.menubar_style = style;
        if (style == MENUBAR_STYLE_GLASS) {
            current_theme.menubar_alpha = 230;
        } else if (style == MENUBAR_STYLE_SOLID) {
            current_theme.menubar_alpha = 255;
        } else if (style == MENUBAR_STYLE_TRANSPARENT) {
            current_theme.menubar_alpha = 160;
        }
    }
}

menubar_style_t theme_get_menubar_style(void) {
    return current_theme.menubar_style;
}

const char* theme_get_menubar_style_name(menubar_style_t style) {
    if (style >= 0 && style < MENUBAR_STYLE_COUNT) return menubar_style_names[style];
    return "Frosted Glass";
}

void theme_set_font_scale(int percent) {
    if (percent < 80) percent = 80;
    if (percent > 150) percent = 150;
    current_theme.font_scale = percent;
    font_set_scale(percent);
}

int theme_get_font_scale(void) {
    return current_theme.font_scale;
}

void theme_update(void) {
    if (current_theme.preset == UI_THEME_AUTO) {
        apply_theme_preset(UI_THEME_AUTO, current_theme.accent_id);
    }
}

// Global UI Scaling Implementation
void ui_scale_set(int percent) {
    if (percent < 50) percent = 50;
    if (percent > 200) percent = 200;
    global_ui_scale_percent = percent;
}

int ui_scale_get(void) {
    return global_ui_scale_percent;
}

void ui_scale_increase(void) {
    for (int i = 0; i < NUM_SCALE_STEPS - 1; i++) {
        if (global_ui_scale_percent == ui_scale_steps[i]) {
            global_ui_scale_percent = ui_scale_steps[i + 1];
            return;
        }
    }
    if (global_ui_scale_percent < 200) global_ui_scale_percent += 25;
}

void ui_scale_decrease(void) {
    for (int i = NUM_SCALE_STEPS - 1; i > 0; i--) {
        if (global_ui_scale_percent == ui_scale_steps[i]) {
            global_ui_scale_percent = ui_scale_steps[i - 1];
            return;
        }
    }
    if (global_ui_scale_percent > 80) global_ui_scale_percent -= 20;
}

int ui_scale_val(int val) {
    if (global_ui_scale_percent == 100) return val;
    return (val * global_ui_scale_percent) / 100;
}

static char scale_label_buf[16];
const char* ui_scale_get_label(void) {
    int sc = global_ui_scale_percent;
    char num[8];
    int idx = 0;
    if (sc >= 100) {
        num[idx++] = '0' + (sc / 100);
        num[idx++] = '0' + ((sc / 10) % 10);
        num[idx++] = '0' + (sc % 10);
    } else {
        num[idx++] = '0' + (sc / 10);
        num[idx++] = '0' + (sc % 10);
    }
    num[idx] = '\0';
    strcpy(scale_label_buf, num);
    strcat(scale_label_buf, "%");
    return scale_label_buf;
}

void ui_theme_set(ui_theme_preset_t preset) {
    theme_set_preset(preset);
}

const ui_theme_t* ui_theme_get(void) {
    return theme_get_current();
}

void ui_theme_next(void) {
    theme_next();
}

const char* ui_theme_get_name(void) {
    return theme_get_preset_name(theme_get_preset());
}

void ui_scale_up(void) {
    ui_scale_increase();
}

void ui_scale_down(void) {
    ui_scale_decrease();
}

int ui_scale_value(void) {
    return ui_scale_get();
}
