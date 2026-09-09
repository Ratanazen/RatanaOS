#include "../include/dock.h"
#include "../include/gfx.h"
#include "../include/icons.h"
#include "../include/theme.h"

static int dock_w = 0;
static int dock_h = 0;
static int dock_x = 0;
static int dock_y = 0;

static int current_icon_size = DOCK_ICON_SIZE;
static int current_spacing = DOCK_SPACING;
static bool magnification_enabled = true;

#define DOCK_APP_COUNT 12

void dock_set_icon_size(int size) {
    if (size >= 32 && size <= 64) {
        current_icon_size = size;
        dock_init();
    }
}

int dock_get_icon_size(void) {
    return current_icon_size;
}

void dock_set_spacing(int spacing) {
    if (spacing >= 2 && spacing <= 16) {
        current_spacing = spacing;
        dock_init();
    }
}

int dock_get_spacing(void) {
    return current_spacing;
}

void dock_set_magnification(bool enabled) {
    magnification_enabled = enabled;
}

bool dock_get_magnification(void) {
    return magnification_enabled;
}

static dock_position_t current_position = DOCK_POS_BOTTOM;
static dock_style_t current_style = DOCK_STYLE_GLASS;
static bool autohide_enabled = false;
static bool dock_visible = true;

void dock_set_position(dock_position_t pos) {
    if (pos >= 0 && pos < DOCK_POS_COUNT) {
        current_position = pos;
        dock_init();
    }
}

dock_position_t dock_get_position(void) {
    return current_position;
}

void dock_set_style(dock_style_t style) {
    if (style >= 0 && style < DOCK_STYLE_COUNT) {
        current_style = style;
        theme_set_dock_style(style);
        dock_init();
    }
}

dock_style_t dock_get_style(void) {
    return current_style;
}

void dock_set_autohide(bool enabled) {
    autohide_enabled = enabled;
    dock_visible = true;
}

bool dock_get_autohide(void) {
    return autohide_enabled;
}

bool dock_is_visible(void) {
    if (!autohide_enabled) return true;
    return dock_visible;
}

void dock_update_cursor(int mx, int my) {
    if (!autohide_enabled) {
        dock_visible = true;
        return;
    }
    int sh = gfx_get_height();
    int sw = gfx_get_width();
    if (current_position == DOCK_POS_BOTTOM) {
        if (my >= sh - 40 || (mx >= dock_x && mx <= dock_x + dock_w && my >= dock_y && my <= dock_y + dock_h)) {
            dock_visible = true;
        } else {
            dock_visible = false;
        }
    } else if (current_position == DOCK_POS_LEFT) {
        if (mx <= 40 || (mx >= dock_x && mx <= dock_x + dock_w && my >= dock_y && my <= dock_y + dock_h)) {
            dock_visible = true;
        } else {
            dock_visible = false;
        }
    } else { // DOCK_POS_RIGHT
        if (mx >= sw - 40 || (mx >= dock_x && mx <= dock_x + dock_w && my >= dock_y && my <= dock_y + dock_h)) {
            dock_visible = true;
        } else {
            dock_visible = false;
        }
    }
}

void dock_init(void) {
    int sw = gfx_get_width();
    int sh = gfx_get_height();

    int icon_s = current_icon_size;
    int pad = UI(DOCK_PADDING);
    int space = current_spacing;

    if (current_position == DOCK_POS_BOTTOM) {
        dock_w = (DOCK_APP_COUNT * (icon_s + space)) + pad * 2 + icon_s + 14;
        dock_h = icon_s + pad * 2;
        dock_x = (sw - dock_w) / 2;
        dock_y = sh - dock_h - UI(8);
    } else if (current_position == DOCK_POS_LEFT) {
        dock_w = icon_s + pad * 2;
        dock_h = (DOCK_APP_COUNT * (icon_s + space)) + pad * 2 + icon_s + 14;
        dock_x = UI(8);
        dock_y = (sh - dock_h) / 2 + 14;
    } else { // DOCK_POS_RIGHT
        dock_w = icon_s + pad * 2;
        dock_h = (DOCK_APP_COUNT * (icon_s + space)) + pad * 2 + icon_s + 14;
        dock_x = sw - dock_w - UI(8);
        dock_y = (sh - dock_h) / 2 + 14;
    }
}

void dock_draw(const bool* window_open_states, const bool* window_active_states, int count) {
    if (!dock_is_visible()) return;

    const ui_theme_t* theme = theme_get_current();
    int icon_s = current_icon_size;
    int pad = UI(DOCK_PADDING);
    int space = current_spacing;
    int radius = theme->dock_radius;

    // 1. Soft Drop Shadow
    if (theme->shadows_enabled) {
        gfx_draw_shadow(dock_x, dock_y, dock_w, dock_h, radius, 6, theme->shadow_alpha);
    }

    // 2. Translucent Frosted Glass Pill Dock
    uint32_t bg_col = theme->dark_mode ? 0x00202028 : 0x00ECECF2;
    uint32_t border_col = theme->dark_mode ? 0x00555562 : 0x00C8C8D4;
    uint32_t rim_col = theme->dark_mode ? 0x00787888 : 0x00FFFFFF;

    if (current_style == DOCK_STYLE_CLASSIC) {
        bg_col = theme->dark_mode ? 0x00181820 : 0x00FFFFFF;
        border_col = theme->border;
    } else if (current_style == DOCK_STYLE_TRANSPARENT) {
        bg_col = theme->dark_mode ? 0x00101014 : 0x00E0E0E6;
    }

    if (theme->transparency_enabled && current_style != DOCK_STYLE_CLASSIC) {
        gfx_draw_rounded_rect_alpha(dock_x, dock_y, dock_w, dock_h, radius, bg_col, theme->dock_alpha);
    } else {
        gfx_draw_rounded_rect(dock_x, dock_y, dock_w, dock_h, radius, bg_col);
    }

    gfx_draw_rounded_rect_outline(dock_x, dock_y, dock_w, dock_h, radius, border_col);
    if (current_position == DOCK_POS_BOTTOM) {
        gfx_draw_line(dock_x + radius, dock_y + 1, dock_x + dock_w - radius, dock_y + 1, rim_col);
    }

    int step = icon_s + space;

    // Render 12 Application Icons
    static const int app_icons[DOCK_APP_COUNT] = {
        ICON_ID_FINDER, ICON_ID_LAUNCHPAD, ICON_ID_SAFARI, ICON_ID_TERMINAL,
        ICON_ID_SYSMON, ICON_ID_CALCULATOR, ICON_ID_PAINT, ICON_ID_NOTES,
        ICON_ID_MUSIC,  ICON_ID_SETTINGS,   ICON_ID_APPSTORE, ICON_ID_ABOUT
    };

    if (current_position == DOCK_POS_BOTTOM) {
        int start_x = dock_x + pad;
        int icon_y = dock_y + pad;

        for (int i = 0; i < DOCK_APP_COUNT; i++) {
            icon_draw_scaled(app_icons[i], start_x + step * i, icon_y, icon_s);
        }

        // Dock Divider Line
        int div_x = start_x + step * DOCK_APP_COUNT - 2;
        uint32_t div_dark = theme->dark_mode ? 0x0050505A : 0x00B0B0B8;
        uint32_t div_light = theme->dark_mode ? 0x00282830 : 0x00E0E0E8;
        gfx_draw_line(div_x, dock_y + 10, div_x, dock_y + dock_h - 10, div_dark);
        gfx_draw_line(div_x + 1, dock_y + 10, div_x + 1, dock_y + dock_h - 10, div_light);

        // 12: Trash Icon
        int trash_x = div_x + 10;
        icon_draw_scaled(ICON_ID_TRASH, trash_x, icon_y, icon_s);

        // Active Running Application Indicator Dots
        for (int i = 0; i < DOCK_APP_COUNT && i < count; i++) {
            if (window_open_states && window_open_states[i]) {
                int dot_x = start_x + i * step + icon_s / 2 - 2;
                int dot_y = dock_y + dock_h - 5;
                uint32_t dot_color = (window_active_states && window_active_states[i]) ? theme->accent : (theme->dark_mode ? COLOR_WHITE : 0x003A3A3C);
                gfx_draw_rect(dot_x, dot_y, 4, 3, dot_color);
            }
        }
    } else { // Vertical Dock (Left or Right)
        int icon_x = dock_x + pad;
        int start_y = dock_y + pad;

        for (int i = 0; i < DOCK_APP_COUNT; i++) {
            icon_draw_scaled(app_icons[i], icon_x, start_y + step * i, icon_s);
        }

        // Divider Line
        int div_y = start_y + step * DOCK_APP_COUNT - 2;
        uint32_t div_dark = theme->dark_mode ? 0x0050505A : 0x00B0B0B8;
        uint32_t div_light = theme->dark_mode ? 0x00282830 : 0x00E0E0E8;
        gfx_draw_line(dock_x + 10, div_y, dock_x + dock_w - 10, div_y, div_dark);
        gfx_draw_line(dock_x + 10, div_y + 1, dock_x + dock_w - 10, div_y + 1, div_light);

        // 12: Trash Icon
        int trash_y = div_y + 10;
        icon_draw_scaled(ICON_ID_TRASH, icon_x, trash_y, icon_s);

        // Active Running Application Indicator Dots
        for (int i = 0; i < DOCK_APP_COUNT && i < count; i++) {
            if (window_open_states && window_open_states[i]) {
                int dot_x = (current_position == DOCK_POS_LEFT) ? (dock_x + 3) : (dock_x + dock_w - 6);
                int dot_y = start_y + i * step + icon_s / 2 - 2;
                uint32_t dot_color = (window_active_states && window_active_states[i]) ? theme->accent : (theme->dark_mode ? COLOR_WHITE : 0x003A3A3C);
                gfx_draw_rect(dot_x, dot_y, 3, 4, dot_color);
            }
        }
    }
}

int dock_hit_test(int mx, int my) {
    if (!dock_is_visible()) return -1;
    if (mx < dock_x || mx > dock_x + dock_w || my < dock_y || my > dock_y + dock_h) {
        return -1;
    }

    int pad = UI(DOCK_PADDING);
    int step = current_icon_size + current_spacing;

    if (current_position == DOCK_POS_BOTTOM) {
        int start_x = dock_x + pad;
        for (int i = 0; i < DOCK_APP_COUNT; i++) {
            int ix = start_x + i * step;
            if (mx >= ix && mx < ix + current_icon_size) {
                return i;
            }
        }
        int div_x = start_x + step * DOCK_APP_COUNT - 2;
        int trash_x = div_x + 10;
        if (mx >= trash_x && mx <= trash_x + current_icon_size) {
            return 12;
        }
    } else { // Vertical Dock
        int start_y = dock_y + pad;
        for (int i = 0; i < DOCK_APP_COUNT; i++) {
            int iy = start_y + i * step;
            if (my >= iy && my < iy + current_icon_size) {
                return i;
            }
        }
        int div_y = start_y + step * DOCK_APP_COUNT - 2;
        int trash_y = div_y + 10;
        if (my >= trash_y && my <= trash_y + current_icon_size) {
            return 12;
        }
    }

    return -1;
}
