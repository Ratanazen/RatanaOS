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

void dock_init(void) {
    int sw = gfx_get_width();
    int sh = gfx_get_height();

    int icon_s = current_icon_size;
    int pad = UI(DOCK_PADDING);
    int space = current_spacing;

    // 12 apps + divider (8px) + 1 trash + padding
    dock_w = (DOCK_APP_COUNT * (icon_s + space)) + pad * 2 + icon_s + 14;
    dock_h = icon_s + pad * 2;
    dock_x = (sw - dock_w) / 2;
    dock_y = sh - dock_h - UI(8);
}

void dock_draw(const bool* window_open_states, const bool* window_active_states, int count) {
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

    if (theme->transparency_enabled) {
        gfx_draw_rounded_rect_alpha(dock_x, dock_y, dock_w, dock_h, radius, bg_col, theme->dock_alpha);
    } else {
        gfx_draw_rounded_rect(dock_x, dock_y, dock_w, dock_h, radius, bg_col);
    }

    gfx_draw_rounded_rect_outline(dock_x, dock_y, dock_w, dock_h, radius, border_col);
    gfx_draw_line(dock_x + radius, dock_y + 1, dock_x + dock_w - radius, dock_y + 1, rim_col);

    // 3. Render 12 Application Icons
    int start_x = dock_x + pad;
    int icon_y = dock_y + pad;
    int step = icon_s + space;

    // 0: Finder
    icon_draw_scaled(ICON_ID_FINDER, start_x + step * 0, icon_y, icon_s);
    // 1: Launchpad
    icon_draw_scaled(ICON_ID_LAUNCHPAD, start_x + step * 1, icon_y, icon_s);
    // 2: Safari
    icon_draw_scaled(ICON_ID_SAFARI, start_x + step * 2, icon_y, icon_s);
    // 3: Terminal
    icon_draw_scaled(ICON_ID_TERMINAL, start_x + step * 3, icon_y, icon_s);
    // 4: Activity Monitor
    icon_draw_scaled(ICON_ID_SYSMON, start_x + step * 4, icon_y, icon_s);
    // 5: Calculator
    icon_draw_scaled(ICON_ID_CALCULATOR, start_x + step * 5, icon_y, icon_s);
    // 6: Paint Studio
    icon_draw_scaled(ICON_ID_PAINT, start_x + step * 6, icon_y, icon_s);
    // 7: Notes
    icon_draw_scaled(ICON_ID_NOTES, start_x + step * 7, icon_y, icon_s);
    // 8: Music
    icon_draw_scaled(ICON_ID_MUSIC, start_x + step * 8, icon_y, icon_s);
    // 9: System Settings
    icon_draw_scaled(ICON_ID_SETTINGS, start_x + step * 9, icon_y, icon_s);
    // 10: App Store
    icon_draw_scaled(ICON_ID_APPSTORE, start_x + step * 10, icon_y, icon_s);
    // 11: About This Mac
    icon_draw_scaled(ICON_ID_ABOUT, start_x + step * 11, icon_y, icon_s);

    // Dock Divider Line
    int div_x = start_x + step * DOCK_APP_COUNT - 2;
    uint32_t div_dark = theme->dark_mode ? 0x0050505A : 0x00B0B0B8;
    uint32_t div_light = theme->dark_mode ? 0x00282830 : 0x00E0E0E8;
    gfx_draw_line(div_x, dock_y + 10, div_x, dock_y + dock_h - 10, div_dark);
    gfx_draw_line(div_x + 1, dock_y + 10, div_x + 1, dock_y + dock_h - 10, div_light);

    // 12: Trash Icon
    int trash_x = div_x + 10;
    icon_draw_scaled(ICON_ID_TRASH, trash_x, icon_y, icon_s);

    // 4. Active Running Application Indicator Dots
    for (int i = 0; i < DOCK_APP_COUNT && i < count; i++) {
        if (window_open_states && window_open_states[i]) {
            int dot_x = start_x + i * step + icon_s / 2 - 2;
            int dot_y = dock_y + dock_h - 5;
            uint32_t dot_color = (window_active_states && window_active_states[i]) ? theme->accent : (theme->dark_mode ? COLOR_WHITE : 0x003A3A3C);
            gfx_draw_rect(dot_x, dot_y, 4, 3, dot_color);
        }
    }
}

int dock_hit_test(int mx, int my) {
    if (mx < dock_x || mx > dock_x + dock_w || my < dock_y || my > dock_y + dock_h) {
        return -1;
    }

    int pad = UI(DOCK_PADDING);
    int start_x = dock_x + pad;
    int step = current_icon_size + current_spacing;

    // Check first 12 app icons
    for (int i = 0; i < DOCK_APP_COUNT; i++) {
        int ix = start_x + i * step;
        if (mx >= ix && mx < ix + current_icon_size) {
            return i;
        }
    }

    // Check Trash icon
    int div_x = start_x + step * DOCK_APP_COUNT - 2;
    int trash_x = div_x + 10;
    if (mx >= trash_x && mx <= trash_x + current_icon_size) {
        return 12;
    }

    return -1;
}
