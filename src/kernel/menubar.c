#include "../include/menubar.h"
#include "../include/gfx.h"
#include "../include/icons.h"
#include "../include/theme.h"
#include "../include/font.h"
#include "../include/rtc.h"
#include "../include/heap.h"
#include "../include/string.h"

static bool apple_menu_open = false;

void menubar_init(void) {
    apple_menu_open = false;
}

bool menubar_is_apple_open(void) {
    return apple_menu_open;
}

void menubar_toggle_apple(void) {
    apple_menu_open = !apple_menu_open;
}

void menubar_close_apple(void) {
    apple_menu_open = false;
}

void menubar_draw(const char* active_app_name) {
    const ui_theme_t* theme = theme_get_current();
    int sw = gfx_get_width();

    // 1. Frosted top bar
    uint32_t bar_bg = theme->dark_mode ? 0x001C1C1E : 0x00F0F0F4;
    uint32_t bar_line = theme->border;

    if (theme->transparency_enabled) {
        gfx_draw_rect_alpha(0, 0, sw, MENUBAR_HEIGHT, bar_bg, theme->menubar_alpha);
    } else {
        gfx_draw_rect(0, 0, sw, MENUBAR_HEIGHT, bar_bg);
    }
    gfx_draw_line(0, MENUBAR_HEIGHT, sw, MENUBAR_HEIGHT, bar_line);

    // 2.  Apple Logo on top-left
    uint32_t apple_col = apple_menu_open ? theme->accent : theme->text_primary;
    icon_draw_apple_logo(12, 4, apple_col);

    // 3. Active Application Title
    const char* app_title = active_app_name ? active_app_name : "Finder";
    font_draw_text(36, 4, app_title, theme->text_primary, FONT_SIZE_REGULAR);

    // 4. Standard Menus: File Edit View Window Help
    int mx = 36 + font_measure_text_width(app_title, FONT_SIZE_REGULAR) + 18;
    font_draw_text(mx, 4, "File", theme->text_secondary, FONT_SIZE_REGULAR);
    font_draw_text(mx + 46, 4, "Edit", theme->text_secondary, FONT_SIZE_REGULAR);
    font_draw_text(mx + 92, 4, "View", theme->text_secondary, FONT_SIZE_REGULAR);
    font_draw_text(mx + 138, 4, "Window", theme->text_secondary, FONT_SIZE_REGULAR);
    font_draw_text(mx + 198, 4, "Help", theme->text_secondary, FONT_SIZE_REGULAR);

    // 5. Right-side Status Extras
    icon_draw_wifi(sw - 286, 8, theme->text_primary);
    icon_draw_battery(sw - 266, 6, theme->text_primary);

    // Dynamic Heap / RAM Widget
    heap_stats_t hstats = heap_get_stats();
    char ram_str[24];
    strcpy(ram_str, "RAM:");
    char rn[12];
    utoa((uint32_t)(hstats.used_size / 1024), rn, 10);
    strcat(ram_str, rn);
    strcat(ram_str, "K");
    font_draw_text(sw - 238, 4, ram_str, 0x0030D158, FONT_SIZE_REGULAR);

    // Live Real-Time Clock
    rtc_time_t t = rtc_get_time();
    char clock_str[32];
    char cn[16];
    utoa(t.month, cn, 10);
    if (t.month < 10) { strcpy(clock_str, "0"); strcat(clock_str, cn); }
    else strcpy(clock_str, cn);
    strcat(clock_str, "/");
    utoa(t.day, cn, 10);
    if (t.day < 10) strcat(clock_str, "0");
    strcat(clock_str, cn);
    strcat(clock_str, " ");
    utoa(t.hour, cn, 10);
    if (t.hour < 10) strcat(clock_str, "0");
    strcat(clock_str, cn);
    strcat(clock_str, ":");
    utoa(t.minute, cn, 10);
    if (t.minute < 10) strcat(clock_str, "0");
    strcat(clock_str, cn);

    font_draw_text(sw - 148, 4, clock_str, theme->text_primary, FONT_SIZE_REGULAR);
    icon_draw_search(sw - 54, 5, theme->text_primary);
    icon_draw_control_center(sw - 36, 6, theme->text_primary);
    icon_draw_siri(sw - 16, 6);

    // 6.  Apple Menu Dropdown
    if (apple_menu_open) {
        int am_w = 210;
        int am_h = 180;
        if (theme->shadows_enabled) {
            gfx_draw_shadow(8, MENUBAR_HEIGHT + 2, am_w, am_h, 8, 6, theme->shadow_alpha);
        }

        uint32_t am_bg = theme->panel_bg;
        if (theme->transparency_enabled) {
            gfx_draw_rounded_rect_alpha(8, MENUBAR_HEIGHT + 2, am_w, am_h, 8, am_bg, theme->panel_alpha);
        } else {
            gfx_draw_rounded_rect(8, MENUBAR_HEIGHT + 2, am_w, am_h, 8, am_bg);
        }
        gfx_draw_rounded_rect_outline(8, MENUBAR_HEIGHT + 2, am_w, am_h, 8, theme->panel_border);

        const char* am_items[] = {
            "About This Mac",
            "System Settings...",
            "App Store...",
            "Recent Items >",
            "Force Quit Applications...",
            "Restart...",
            "Shut Down...",
            "Exit to CLI"
        };

        for (int i = 0; i < 8; i++) {
            uint32_t col = (i == 7) ? COLOR_RED : theme->text_primary;
            font_draw_text(18, MENUBAR_HEIGHT + 10 + i * 20, am_items[i], col, FONT_SIZE_REGULAR);
        }
    }
}

menubar_hit_t menubar_hit_test(int mx, int my) {
    if (apple_menu_open) {
        if (mx >= 8 && mx <= 218 && my >= MENUBAR_HEIGHT && my <= MENUBAR_HEIGHT + 180) {
            int item = (my - (MENUBAR_HEIGHT + 10)) / 20;
            apple_menu_open = false;
            if (item == 0) return MENUBAR_HIT_ITEM_ABOUT;
            if (item == 1) return MENUBAR_HIT_ITEM_SETTINGS;
            if (item == 7) return MENUBAR_HIT_ITEM_EXIT_CLI;
            return MENUBAR_HIT_NONE;
        }
    }

    if (my >= 0 && my <= MENUBAR_HEIGHT) {
        if (mx >= 8 && mx <= 30) {
            return MENUBAR_HIT_APPLE_LOGO;
        }
    }

    return MENUBAR_HIT_NONE;
}
