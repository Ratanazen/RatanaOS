#include "../include/dock.h"
#include "../include/gfx.h"
#include "../include/icons.h"

static int dock_w = 0;
static int dock_h = 0;
static int dock_x = 0;
static int dock_y = 0;

#define DOCK_APP_COUNT 12

void dock_init(void) {
    int sw = gfx_get_width();
    int sh = gfx_get_height();

    // 12 apps + divider (8px) + 1 trash + padding
    dock_w = (DOCK_APP_COUNT * (DOCK_ICON_SIZE + DOCK_SPACING)) + DOCK_PADDING * 2 + DOCK_ICON_SIZE + 14;
    dock_h = DOCK_ICON_SIZE + DOCK_PADDING * 2;
    dock_x = (sw - dock_w) / 2;
    dock_y = sh - dock_h - 8;
}

void dock_draw(const bool* window_open_states, const bool* window_active_states, int count) {
    // 1. Translucent Frosted Glass Pill Dock (alpha blend 215/255)
    gfx_draw_rounded_rect_alpha(dock_x, dock_y, dock_w, dock_h, DOCK_RADIUS, 0x00202028, 215);
    gfx_draw_rounded_rect_outline(dock_x, dock_y, dock_w, dock_h, DOCK_RADIUS, 0x00555562);
    gfx_draw_line(dock_x + DOCK_RADIUS, dock_y + 1, dock_x + dock_w - DOCK_RADIUS, dock_y + 1, 0x00787888);

    // 2. Render 12 Application Icons
    int start_x = dock_x + DOCK_PADDING;
    int icon_y = dock_y + DOCK_PADDING;
    int step = DOCK_ICON_SIZE + DOCK_SPACING;

    // 0: Finder
    icon_draw_finder_48(start_x + step * 0, icon_y);
    // 1: Launchpad
    icon_draw_launchpad_48(start_x + step * 1, icon_y);
    // 2: Safari
    icon_draw_safari_48(start_x + step * 2, icon_y);
    // 3: Terminal
    icon_draw_terminal_48(start_x + step * 3, icon_y);
    // 4: Activity Monitor
    icon_draw_sysmon_48(start_x + step * 4, icon_y);
    // 5: Calculator
    icon_draw_calculator_48(start_x + step * 5, icon_y);
    // 6: Paint Studio
    icon_draw_paint_48(start_x + step * 6, icon_y);
    // 7: Notes
    icon_draw_notes_48(start_x + step * 7, icon_y);
    // 8: Music
    icon_draw_music_48(start_x + step * 8, icon_y);
    // 9: System Settings
    icon_draw_settings_48(start_x + step * 9, icon_y);
    // 10: App Store
    icon_draw_appstore_48(start_x + step * 10, icon_y);
    // 11: About This Mac
    icon_draw_about_48(start_x + step * 11, icon_y);

    // Dock Divider Line
    int div_x = start_x + step * DOCK_APP_COUNT - 2;
    gfx_draw_line(div_x, dock_y + 10, div_x, dock_y + dock_h - 10, 0x0050505A);
    gfx_draw_line(div_x + 1, dock_y + 10, div_x + 1, dock_y + dock_h - 10, 0x00282830);

    // 12: Trash Icon
    int trash_x = div_x + 10;
    icon_draw_trash_48(trash_x, icon_y);

    // 3. Active Running Application Indicator Dots
    for (int i = 0; i < DOCK_APP_COUNT && i < count; i++) {
        if (window_open_states && window_open_states[i]) {
            int dot_x = start_x + i * step + DOCK_ICON_SIZE / 2 - 2;
            int dot_y = dock_y + dock_h - 5;
            uint32_t dot_color = (window_active_states && window_active_states[i]) ? 0x000A84FF : COLOR_WHITE;
            gfx_draw_rect(dot_x, dot_y, 4, 3, dot_color);
        }
    }
}

int dock_hit_test(int mx, int my) {
    if (mx < dock_x || mx > dock_x + dock_w || my < dock_y || my > dock_y + dock_h) {
        return -1;
    }

    int start_x = dock_x + DOCK_PADDING;
    int step = DOCK_ICON_SIZE + DOCK_SPACING;

    // Check first 12 app icons
    for (int i = 0; i < DOCK_APP_COUNT; i++) {
        int ix = start_x + i * step;
        if (mx >= ix && mx < ix + DOCK_ICON_SIZE) {
            return i;
        }
    }

    // Check Trash icon
    int div_x = start_x + step * DOCK_APP_COUNT - 2;
    int trash_x = div_x + 10;
    if (mx >= trash_x && mx <= trash_x + DOCK_ICON_SIZE) {
        return 12;
    }

    return -1;
}
