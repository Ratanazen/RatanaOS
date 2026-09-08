#include "../include/menubar.h"
#include "../include/gfx.h"
#include "../include/icons.h"
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
    int sw = gfx_get_width();

    // 1. Semi-transparent dark frosted top bar (24px)
    gfx_draw_rect_alpha(0, 0, sw, MENUBAR_HEIGHT, 0x001C1C1E, 230);
    gfx_draw_line(0, MENUBAR_HEIGHT, sw, MENUBAR_HEIGHT, 0x003A3A3C);

    // 2.  Apple Logo on top-left
    icon_draw_apple_logo(12, 4, apple_menu_open ? 0x000A84FF : COLOR_WHITE);

    // 3. Bold Active Application Title
    const char* app_title = active_app_name ? active_app_name : "Finder";
    gfx_draw_string_transparent(36, 4, app_title, COLOR_WHITE);

    // 4. Standard Menus: File Edit View Window Help
    int mx = 36 + (int)strlen(app_title) * 8 + 18;
    gfx_draw_string_transparent(mx, 4, "File", 0x00D1D1D6);
    gfx_draw_string_transparent(mx + 46, 4, "Edit", 0x00D1D1D6);
    gfx_draw_string_transparent(mx + 92, 4, "View", 0x00D1D1D6);
    gfx_draw_string_transparent(mx + 138, 4, "Window", 0x00D1D1D6);
    gfx_draw_string_transparent(mx + 198, 4, "Help", 0x00D1D1D6);

    // 5. Right-side Status Extras
    icon_draw_wifi(sw - 286, 8, COLOR_WHITE);
    icon_draw_battery(sw - 266, 6, COLOR_WHITE);

    // Dynamic Heap / RAM Widget
    heap_stats_t hstats = heap_get_stats();
    char ram_str[24];
    strcpy(ram_str, "RAM:");
    char rn[12];
    utoa((uint32_t)(hstats.used_size / 1024), rn, 10);
    strcat(ram_str, rn);
    strcat(ram_str, "K");
    gfx_draw_string_transparent(sw - 238, 4, ram_str, 0x0030D158);

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

    gfx_draw_string_transparent(sw - 148, 4, clock_str, COLOR_WHITE);
    icon_draw_search(sw - 54, 5, COLOR_WHITE);
    icon_draw_control_center(sw - 36, 6, COLOR_WHITE);
    icon_draw_siri(sw - 16, 6);

    // 6.  Apple Menu Dropdown
    if (apple_menu_open) {
        int am_w = 210;
        int am_h = 180;
        gfx_draw_rounded_rect_alpha(8, MENUBAR_HEIGHT + 2, am_w, am_h, 8, 0x00242428, 240);
        gfx_draw_rounded_rect_outline(8, MENUBAR_HEIGHT + 2, am_w, am_h, 8, 0x00484852);

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
            uint32_t col = (i == 7) ? 0x00FF453A : COLOR_WHITE;
            gfx_draw_string_transparent(18, MENUBAR_HEIGHT + 10 + i * 20, am_items[i], col);
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
