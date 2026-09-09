#include "../include/gui.h"
#include "../include/gfx.h"
#include "../include/icons.h"
#include "../include/theme.h"
#include "../include/font.h"
#include "../include/ui_controls.h"
#include "../include/settings.h"
#include "../include/menubar.h"
#include "../include/dock.h"
#include "../include/mouse.h"
#include "../include/keyboard.h"
#include "../include/rtc.h"
#include "../include/heap.h"
#include "../include/physical.h"
#include "../include/cpuid.h"
#include "../include/timer.h"
#include "../include/string.h"
#include "../include/stdio.h"
#include "../include/vga.h"

#define TITLEBAR_HEIGHT 28

static window_t windows[MAX_WINDOWS];
static int window_count = 0;
static bool gui_running = false;
static bool prev_left_click = false;

// Calculator state
static char calc_display[32] = "0";
static int  calc_op1 = 0;
static char calc_op = 0;
static bool calc_clear_on_next = false;

// Paint Canvas state (340x190 pixels)
#define CANVAS_W 340
#define CANVAS_H 190
static uint32_t paint_canvas[CANVAS_W * CANVAS_H];
static uint32_t paint_cur_color = 0x000A84FF; // macOS Blue

// Forward declarations of window drawing callbacks
static void draw_finder_content(window_t* win);
static void draw_launchpad_content(window_t* win);
static void draw_safari_content(window_t* win);
static void draw_terminal_content(window_t* win);
static void draw_sysmon_content(window_t* win);
static void draw_calc_content(window_t* win);
static void draw_paint_content(window_t* win);
static void draw_notes_content(window_t* win);
static void draw_music_content(window_t* win);
static void draw_settings_content(window_t* win);
static void draw_appstore_content(window_t* win);
static void draw_about_content(window_t* win);
static void draw_telegram_content(window_t* win);

static void handle_calc_click(window_t* win, int rel_x, int rel_y, int btn);
static void handle_paint_click(window_t* win, int rel_x, int rel_y, int btn);
static void handle_settings_click(window_t* win, int rel_x, int rel_y, int btn);

static window_t* window_by_id(int id) {
    for (int i = 0; i < window_count; i++) {
        if (windows[i].id == id) return &windows[i];
    }
    return NULL;
}

static window_t* create_window(int id, const char* title, int x, int y, int w, int h,
                              void (*draw)(window_t*), void (*click)(window_t*, int, int, int)) {
    if (window_count >= MAX_WINDOWS) return NULL;
    window_t* win = &windows[window_count++];
    win->id = id;
    strncpy(win->title, title, sizeof(win->title) - 1);
    win->x = x;
    win->y = y;
    win->width = w;
    win->height = h;
    win->orig_w = w;
    win->orig_h = h;
    win->is_open = false;
    win->is_minimized = false;
    win->is_active = false;
    win->is_dragging = false;
    win->draw_content = draw;
    win->handle_click = click;
    return win;
}

static void focus_window(int idx) {
    for (int i = 0; i < window_count; i++) {
        windows[i].is_active = (i == idx);
    }
    if (idx >= 0 && idx < window_count) {
        window_t temp = windows[idx];
        for (int i = idx; i < window_count - 1; i++) {
            windows[i] = windows[i + 1];
        }
        windows[window_count - 1] = temp;
        windows[window_count - 1].is_active = true;
    }
}

static void focus_window_by_id(int id) {
    for (int i = 0; i < window_count; i++) {
        if (windows[i].id == id) {
            focus_window(i);
            return;
        }
    }
}

static void show_window(int id) {
    window_t* win = window_by_id(id);
    if (!win) return;
    win->is_open = true;
    win->is_minimized = false;
    focus_window_by_id(id);
}

// -------------------------------------------------------------
// §3. macOS Window Chrome & Traffic Lights (🔴 🟡 🟢)
// -------------------------------------------------------------
static titlebar_hit_t window_hit_test_titlebar(window_t* win, int mx, int my) {
    if (mx < win->x || mx >= win->x + win->width || my < win->y || my >= win->y + TITLEBAR_HEIGHT) {
        return TITLEBAR_HIT_NONE;
    }

    int btn_cy = win->y + 14;

    // 🔴 Red Close Button
    int dx = mx - (win->x + 14), dy = my - btn_cy;
    if (dx * dx + dy * dy <= 36) return TITLEBAR_HIT_CLOSE;

    // 🟡 Yellow Minimize Button
    dx = mx - (win->x + 32);
    if (dx * dx + dy * dy <= 36) return TITLEBAR_HIT_MINIMIZE;

    // 🟢 Green Zoom Button
    dx = mx - (win->x + 50);
    if (dx * dx + dy * dy <= 36) return TITLEBAR_HIT_ZOOM;

    return TITLEBAR_HIT_DRAG;
}

static void draw_window(window_t* win) {
    if (!win->is_open || win->is_minimized) return;

    const ui_theme_t* theme = theme_get_current();
    int r = theme->window_radius;

    // 1. Soft Drop Shadow
    if (theme->shadows_enabled) {
        gfx_draw_shadow(win->x, win->y, win->width, win->height, r, theme->window_shadow_size, theme->shadow_alpha);
    }

    // 2. Main Window Frame
    uint32_t bg_col = theme->window_bg;
    uint32_t border_col = win->is_active ? theme->window_border_active : theme->window_border_inactive;

    if (theme->transparency_enabled) {
        gfx_draw_rounded_rect_alpha(win->x, win->y, win->width, win->height, r, bg_col, theme->window_alpha);
    } else {
        gfx_draw_rounded_rect(win->x, win->y, win->width, win->height, r, bg_col);
    }
    gfx_draw_rounded_rect_outline(win->x, win->y, win->width, win->height, r, border_col);

    // 3. macOS Titlebar
    gfx_draw_rounded_rect(win->x + 1, win->y + 1, win->width - 2, TITLEBAR_HEIGHT, (r > 1 ? r - 1 : 1), theme->window_titlebar);
    gfx_draw_line(win->x, win->y + TITLEBAR_HEIGHT, win->x + win->width - 1, win->y + TITLEBAR_HEIGHT, theme->border);

    // 4. 🔴 🟡 🟢 Traffic Lights (12px Diameter)
    if (win->is_active) {
        gfx_draw_circle(win->x + 14, win->y + 14, 6, COLOR_MAC_CLOSE);
        gfx_draw_circle(win->x + 32, win->y + 14, 6, COLOR_MAC_MIN);
        gfx_draw_circle(win->x + 50, win->y + 14, 6, COLOR_MAC_ZOOM);
    } else {
        // Inactive grayscale traffic lights
        uint32_t inact = theme->dark_mode ? 0x004A4A50 : 0x00C0C0C8;
        gfx_draw_circle(win->x + 14, win->y + 14, 6, inact);
        gfx_draw_circle(win->x + 32, win->y + 14, 6, inact);
        gfx_draw_circle(win->x + 50, win->y + 14, 6, inact);
    }

    // 5. Centered Window Title Text
    uint32_t title_col = win->is_active ? theme->window_title_active : theme->window_title_inactive;
    int tw = font_measure_text_width(win->title, FONT_SIZE_REGULAR);
    int title_x = win->x + (win->width - tw) / 2;
    font_draw_text(title_x, win->y + 6, win->title, title_col, FONT_SIZE_REGULAR);

    // 6. Application Content with Viewport Clipping
    if (win->draw_content) {
        gfx_set_clip(win->x + 1, win->y + TITLEBAR_HEIGHT + 1, win->width - 2, win->height - TITLEBAR_HEIGHT - 2);
        win->draw_content(win);
        gfx_reset_clip();
    }
}

// -------------------------------------------------------------
// App 0: Finder Window
// -------------------------------------------------------------
static void draw_finder_content(window_t* win) {
    const ui_theme_t* theme = theme_get_current();
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    int sidebar_w = 124;
    gfx_draw_rect(cx, cy, sidebar_w, ch, theme->sidebar_bg);
    gfx_draw_line(cx + sidebar_w, cy, cx + sidebar_w, cy + ch, theme->sidebar_border);

    font_draw_text(cx + 10, cy + 10, "Favorites", theme->text_secondary, FONT_SIZE_SMALL);
    font_draw_text(cx + 14, cy + 30, "> Applications", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(cx + 14, cy + 50, "> Desktop", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(cx + 14, cy + 70, "> Documents", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(cx + 14, cy + 90, "> Downloads", theme->text_primary, FONT_SIZE_REGULAR);

    font_draw_text(cx + 10, cy + 120, "Locations", theme->text_secondary, FONT_SIZE_SMALL);
    font_draw_text(cx + 14, cy + 140, "[=] RatanaOS HD", theme->accent, FONT_SIZE_REGULAR);

    int rx = cx + sidebar_w + 14;
    gfx_draw_rect(cx + sidebar_w + 1, cy, cw - sidebar_w - 1, ch, theme->panel_bg);

    // Row 1 of Applications
    icon_draw_safari_48(rx, cy + 14);
    font_draw_text(rx + 2, cy + 54, "Safari", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_terminal_48(rx + 80, cy + 14);
    font_draw_text(rx + 74, cy + 54, "Terminal", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_sysmon_48(rx + 160, cy + 14);
    font_draw_text(rx + 154, cy + 54, "Activity", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_calculator_48(rx + 240, cy + 14);
    font_draw_text(rx + 234, cy + 54, "Calculator", theme->text_primary, FONT_SIZE_REGULAR);

    // Row 2 of Applications
    icon_draw_paint_48(rx, cy + 84);
    font_draw_text(rx + 6, cy + 124, "Paint", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_notes_48(rx + 80, cy + 84);
    font_draw_text(rx + 84, cy + 124, "Notes", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_music_48(rx + 160, cy + 84);
    font_draw_text(rx + 164, cy + 124, "Music", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_settings_48(rx + 240, cy + 84);
    font_draw_text(rx + 236, cy + 124, "Settings", theme->text_primary, FONT_SIZE_REGULAR);

    gfx_draw_line(cx + sidebar_w, cy + ch - 24, cx + cw, cy + ch - 24, theme->border);
    font_draw_text(rx, cy + ch - 18, "8 items, 256 MB available on RatanaOS HD", theme->text_secondary, FONT_SIZE_SMALL);
}

// -------------------------------------------------------------
// App 1: Launchpad Modal Window (520x360)
// -------------------------------------------------------------
static void draw_launchpad_content(window_t* win) {
    const ui_theme_t* theme = theme_get_current();
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    gfx_draw_rect(cx, cy, cw, ch, theme->panel_bg);

    // Search Bar Pill
    gfx_draw_rounded_rect(cx + 120, cy + 10, cw - 240, 24, 12, theme->control_bg);
    gfx_draw_rounded_rect_outline(cx + 120, cy + 10, cw - 240, 24, 12, theme->control_border);
    icon_draw_search(cx + 130, cy + 14, theme->text_secondary);
    font_draw_text(cx + 150, cy + 14, "Search Applications...", theme->text_secondary, FONT_SIZE_REGULAR);

    // 4x3 Grid of App Icons
    int grid_x = cx + 36;
    int grid_y = cy + 46;
    int step_x = 112;
    int step_y = 80;

    // Row 0
    icon_draw_finder_48(grid_x + step_x * 0, grid_y + step_y * 0);
    font_draw_text(grid_x + step_x * 0 + 2, grid_y + step_y * 0 + 52, "Finder", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_safari_48(grid_x + step_x * 1, grid_y + step_y * 0);
    font_draw_text(grid_x + step_x * 1 + 2, grid_y + step_y * 0 + 52, "Safari", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_terminal_48(grid_x + step_x * 2, grid_y + step_y * 0);
    font_draw_text(grid_x + step_x * 2 - 4, grid_y + step_y * 0 + 52, "Terminal", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_sysmon_48(grid_x + step_x * 3, grid_y + step_y * 0);
    font_draw_text(grid_x + step_x * 3 - 6, grid_y + step_y * 0 + 52, "Activity", theme->text_primary, FONT_SIZE_REGULAR);

    // Row 1
    icon_draw_calculator_48(grid_x + step_x * 0, grid_y + step_y * 1);
    font_draw_text(grid_x + step_x * 0 - 6, grid_y + step_y * 1 + 52, "Calculator", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_paint_48(grid_x + step_x * 1, grid_y + step_y * 1);
    font_draw_text(grid_x + step_x * 1 + 6, grid_y + step_y * 1 + 52, "Paint", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_notes_48(grid_x + step_x * 2, grid_y + step_y * 1);
    font_draw_text(grid_x + step_x * 2 + 6, grid_y + step_y * 1 + 52, "Notes", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_music_48(grid_x + step_x * 3, grid_y + step_y * 1);
    font_draw_text(grid_x + step_x * 3 + 6, grid_y + step_y * 1 + 52, "Music", theme->text_primary, FONT_SIZE_REGULAR);

    // Row 2
    icon_draw_settings_48(grid_x + step_x * 0, grid_y + step_y * 2);
    font_draw_text(grid_x + step_x * 0 - 2, grid_y + step_y * 2 + 52, "Settings", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_appstore_48(grid_x + step_x * 1, grid_y + step_y * 2);
    font_draw_text(grid_x + step_x * 1 - 2, grid_y + step_y * 2 + 52, "App Store", theme->text_primary, FONT_SIZE_REGULAR);

    icon_draw_about_48(grid_x + step_x * 2, grid_y + step_y * 2);
    font_draw_text(grid_x + step_x * 2 + 6, grid_y + step_y * 2 + 52, "About", theme->text_primary, FONT_SIZE_REGULAR);
}

// -------------------------------------------------------------
// App 2: Safari Browser Window (560x360)
// -------------------------------------------------------------
static void draw_safari_content(window_t* win) {
    const ui_theme_t* theme = theme_get_current();
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    // Toolbar (#202024)
    gfx_draw_rect(cx, cy, cw, 36, theme->panel_bg);
    gfx_draw_line(cx, cy + 36, cx + cw, cy + 36, theme->border);

    font_draw_text(cx + 12, cy + 10, "<", theme->text_secondary, FONT_SIZE_REGULAR);
    font_draw_text(cx + 28, cy + 10, ">", theme->text_secondary, FONT_SIZE_REGULAR);

    // URL Bar Pill
    int url_w = cw - 120;
    gfx_draw_rounded_rect(cx + 50, cy + 6, url_w, 24, 12, theme->control_bg);
    gfx_draw_rounded_rect_outline(cx + 50, cy + 6, url_w, 24, 12, theme->control_border);
    font_draw_text(cx + 64, cy + 10, "[*] https://ratanaos.org/developer/docs", theme->text_primary, FONT_SIZE_REGULAR);

    // Page Content Area
    gfx_draw_rect(cx, cy + 37, cw, ch - 37, theme->dark_mode ? 0x00141418 : 0x00FFFFFF);

    int px = cx + 24;
    int py = cy + 54;
    font_draw_text(px, py, "Welcome to RatanaOS Safari", theme->accent, FONT_SIZE_LARGE);
    font_draw_text(px, py + 26, "The Next-Generation 64-bit Operating System Desktop.", theme->text_secondary, FONT_SIZE_REGULAR);

    // Feature Cards
    gfx_draw_rounded_rect(px, py + 54, 230, 80, 8, theme->dark_mode ? 0x001E1E24 : 0x00F0F0F6);
    gfx_draw_rounded_rect_outline(px, py + 54, 230, 80, 8, theme->border);
    font_draw_text(px + 12, py + 66, "Native 64-bit Architecture", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(px + 12, py + 86, "x86-64 Long Mode with GDT64,", theme->text_secondary, FONT_SIZE_SMALL);
    font_draw_text(px + 12, py + 100, "IDT64 and 4-Level Paging.", theme->text_secondary, FONT_SIZE_SMALL);

    gfx_draw_rounded_rect(px + 246, py + 54, 230, 80, 8, theme->dark_mode ? 0x001E1E24 : 0x00F0F0F6);
    gfx_draw_rounded_rect_outline(px + 246, py + 54, 230, 80, 8, theme->border);
    font_draw_text(px + 258, py + 66, "Theme Engine & Design System", theme->accent, FONT_SIZE_REGULAR);
    font_draw_text(px + 258, py + 86, "32-bit ARGB Alpha Blending,", theme->text_secondary, FONT_SIZE_SMALL);
    font_draw_text(px + 258, py + 100, "WhiteSur & MacTahoe Icons.", theme->text_secondary, FONT_SIZE_SMALL);
}

// -------------------------------------------------------------
// App 3: Terminal.app Window
// -------------------------------------------------------------
static void draw_terminal_content(window_t* win) {
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    // Deep Dark Console (#0C0C0E)
    gfx_draw_rect(cx, cy, cw, ch, 0x000C0C0E);

    int tx = cx + 8;
    int ty = cy + 8;
    font_draw_text(tx, ty, "Last login: Tue Sep 8 2026 on console", 0x008E8E93, FONT_SIZE_REGULAR);
    font_draw_text(tx, ty + 18, "ratana@os-x86_64:~$ uname -a", 0x0030D158, FONT_SIZE_REGULAR);
    font_draw_text(tx, ty + 36, "RatanaOS 15.4 x86_64 LongMode SMP 2026", 0x00FFFFFF, FONT_SIZE_REGULAR);
    font_draw_text(tx, ty + 54, "ratana@os-x86_64:~$ fetch", 0x0030D158, FONT_SIZE_REGULAR);
    font_draw_text(tx, ty + 72, "OS: RatanaOS 64-bit macOS Sequoia Edition", 0x005AC8FA, FONT_SIZE_REGULAR);
    font_draw_text(tx, ty + 90, "Kernel: 4-Level Paging 2MB Huge Pages", 0x00FFFFFF, FONT_SIZE_REGULAR);
    font_draw_text(tx, ty + 108, "UI: Theme Engine + UI Design System Active", 0x00FFD60A, FONT_SIZE_REGULAR);

    font_draw_text(tx, ty + 130, "ratana@os-x86_64:~$ _", 0x0030D158, FONT_SIZE_REGULAR);
}

// -------------------------------------------------------------
// App 4: Activity Monitor.app Window
// -------------------------------------------------------------
static void draw_sysmon_content(window_t* win) {
    const ui_theme_t* theme = theme_get_current();
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    gfx_draw_rect(cx, cy, cw, ch, theme->panel_bg);

    // Tab Header
    gfx_draw_rect(cx, cy, cw, 28, theme->sidebar_bg);
    gfx_draw_line(cx, cy + 28, cx + cw, cy + 28, theme->border);
    font_draw_text(cx + 12, cy + 6, "[ CPU ]   [ Memory ]   [ Energy ]   [ Disk ]   [ Network ]", theme->text_primary, FONT_SIZE_REGULAR);

    // CPU Oscilloscope Graph
    int gx = cx + 12;
    int gy = cy + 36;
    int gw = cw - 24;
    int gh = 64;
    gfx_draw_rect(gx, gy, gw, gh, 0x00101014);
    gfx_draw_rect_outline(gx, gy, gw, gh, 0x0030D158);

    for (int x = 0; x < gw; x += 4) {
        int bar_h = ((x * 13) % (gh - 10)) + 4;
        gfx_draw_rect(gx + x, gy + gh - bar_h, 3, bar_h, 0x0030D158);
    }

    // Process Table
    int py = cy + 110;
    gfx_draw_rect(cx, py, cw, 20, theme->sidebar_bg);
    font_draw_text(cx + 12, py + 2, "Process Name          PID     CPU%    Memory     State", theme->text_secondary, FONT_SIZE_REGULAR);
    gfx_draw_line(cx, py + 20, cx + cw, py + 20, theme->border);

    font_draw_text(cx + 12, py + 24, "kernel_main           0       1.2%    16.0 MB    Running", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(cx + 12, py + 42, "compositor_60fps      1       0.8%    3.1 MB     Running", theme->accent, FONT_SIZE_REGULAR);
    font_draw_text(cx + 12, py + 60, "theme_engine          2       0.1%    512 KB     Running", 0x0030D158, FONT_SIZE_REGULAR);
    font_draw_text(cx + 12, py + 78, "dock_manager          3       0.2%    1.2 MB     Sleeping", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(cx + 12, py + 96, "ps2_input_server      4       0.1%    256 KB     Idle", theme->text_primary, FONT_SIZE_REGULAR);
}

// -------------------------------------------------------------
// App 5: Calculator.app Window
// -------------------------------------------------------------
static const char* calc_buttons[4][4] = {
    {"C", "+/-", "%", "/"},
    {"7", "8",   "9", "*"},
    {"4", "5",   "6", "-"},
    {"1", "2",   "3", "+"}
};

static void draw_calc_content(window_t* win) {
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    gfx_draw_rect(cx, cy, cw, ch, 0x001C1C1E);

    // Display LCD
    gfx_draw_rect(cx + 8, cy + 8, cw - 16, 40, 0x002C2C2E);
    gfx_draw_rect_outline(cx + 8, cy + 8, cw - 16, 40, 0x003A3A3C);
    int disp_tw = font_measure_text_width(calc_display, FONT_SIZE_LARGE);
    font_draw_text(cx + cw - 24 - disp_tw, cy + 16, calc_display, COLOR_WHITE, FONT_SIZE_LARGE);

    // Keypad (4x5)
    int btn_w = 48;
    int btn_h = 34;
    int k_y = cy + 56;

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int bx = cx + 10 + c * (btn_w + 8);
            int by = k_y + r * (btn_h + 8);
            uint32_t col = (c == 3) ? 0x00FF9F0A : (r == 0 ? 0x00505054 : 0x003A3A3C);
            gfx_draw_rounded_rect(bx, by, btn_w, btn_h, 6, col);
            font_draw_text_center(bx, by, btn_w, btn_h, calc_buttons[r][c], COLOR_WHITE, FONT_SIZE_REGULAR);
        }
    }

    // Bottom Row: 0 (wide) and =
    int by = k_y + 4 * (btn_h + 8);
    gfx_draw_rounded_rect(cx + 10, by, btn_w * 2 + 8, btn_h, 6, 0x003A3A3C);
    font_draw_text(cx + 26, by + 9, "0", COLOR_WHITE, FONT_SIZE_REGULAR);

    gfx_draw_rounded_rect(cx + 10 + 3 * (btn_w + 8), by, btn_w, btn_h, 6, 0x00FF9F0A);
    font_draw_text_center(cx + 10 + 3 * (btn_w + 8), by, btn_w, btn_h, "=", COLOR_WHITE, FONT_SIZE_REGULAR);
}

static void handle_calc_click(window_t* win, int rel_x, int rel_y, int btn) {
    (void)win; (void)btn;
    int cx = 10;
    int cy = TITLEBAR_HEIGHT + 1 + 56;
    int btn_w = 48;
    int btn_h = 34;

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int bx = cx + c * (btn_w + 8);
            int by = cy + r * (btn_h + 8);

            if (rel_x >= bx && rel_x < bx + btn_w && rel_y >= by && rel_y < by + btn_h) {
                const char* label = calc_buttons[r][c];
                if (strcmp(label, "C") == 0) {
                    strcpy(calc_display, "0");
                    calc_op1 = 0;
                    calc_op = 0;
                    calc_clear_on_next = false;
                } else if (label[0] >= '0' && label[0] <= '9') {
                    if (calc_clear_on_next || strcmp(calc_display, "0") == 0) {
                        calc_display[0] = label[0];
                        calc_display[1] = '\0';
                        calc_clear_on_next = false;
                    } else if (strlen(calc_display) < 10) {
                        strcat(calc_display, label);
                    }
                } else if (label[0] == '+' || label[0] == '-' || label[0] == '*' || label[0] == '/') {
                    calc_op1 = atoi(calc_display);
                    calc_op = label[0];
                    calc_clear_on_next = true;
                }
                return;
            }
        }
    }

    int by = cy + 4 * (btn_h + 8);
    if (rel_y >= by && rel_y < by + btn_h) {
        if (rel_x >= cx && rel_x < cx + 104) {
            if (calc_clear_on_next || strcmp(calc_display, "0") == 0) {
                strcpy(calc_display, "0");
                calc_clear_on_next = false;
            } else if (strlen(calc_display) < 10) {
                strcat(calc_display, "0");
            }
        } else if (rel_x >= cx + 3 * (btn_w + 8) && rel_x < cx + 3 * (btn_w + 8) + btn_w) {
            if (calc_op != 0) {
                int op2 = atoi(calc_display);
                int res = 0;
                if (calc_op == '+') res = calc_op1 + op2;
                if (calc_op == '-') res = calc_op1 - op2;
                if (calc_op == '*') res = calc_op1 * op2;
                if (calc_op == '/') res = (op2 != 0) ? (calc_op1 / op2) : 0;
                itoa(res, calc_display, 10);
                calc_op = 0;
                calc_clear_on_next = true;
            }
        }
    }
}

// -------------------------------------------------------------
// App 6: Paint Studio.app Window
// -------------------------------------------------------------
static const uint32_t paint_colors[8] = {
    0x00FFFFFF, 0x00FF453A, 0x0030D158, 0x000A84FF,
    0x00FFD60A, 0x005AC8FA, 0x00BF5AF2, 0x001C1C1E
};

static void draw_paint_content(window_t* win) {
    int cx = win->x + 8;
    int cy = win->y + TITLEBAR_HEIGHT + 8;

    for (int i = 0; i < 8; i++) {
        int px = cx + i * 28;
        int py = cy;
        gfx_draw_rounded_rect(px, py, 22, 22, 4, paint_colors[i]);
        gfx_draw_rounded_rect_outline(px, py, 22, 22, 4, (paint_cur_color == paint_colors[i]) ? COLOR_WHITE : 0x003A3A3C);
    }

    int can_y = cy + 30;
    gfx_draw_rect_outline(cx - 1, can_y - 1, CANVAS_W + 2, CANVAS_H + 2, 0x003A3A3C);

    for (int y = 0; y < CANVAS_H; y++) {
        for (int x = 0; x < CANVAS_W; x++) {
            uint32_t col = paint_canvas[y * CANVAS_W + x];
            if (col != 0) {
                gfx_draw_pixel(cx + x, can_y + y, col);
            } else {
                gfx_draw_pixel(cx + x, can_y + y, 0x00202024);
            }
        }
    }
}

static void handle_paint_click(window_t* win, int rel_x, int rel_y, int btn) {
    (void)win; (void)btn;
    int cx = 8;
    int cy = TITLEBAR_HEIGHT + 8;

    for (int i = 0; i < 8; i++) {
        int px = cx + i * 28;
        if (rel_x >= px && rel_x < px + 22 && rel_y >= cy && rel_y < cy + 22) {
            paint_cur_color = paint_colors[i];
            return;
        }
    }

    int can_y = cy + 30;
    int draw_x = rel_x - cx;
    int draw_y = rel_y - can_y;

    if (draw_x >= 0 && draw_x < CANVAS_W && draw_y >= 0 && draw_y < CANVAS_H) {
        for (int dy = -2; dy <= 2; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
                int px = draw_x + dx;
                int py = draw_y + dy;
                if (px >= 0 && px < CANVAS_W && py >= 0 && py < CANVAS_H) {
                    paint_canvas[py * CANVAS_W + px] = paint_cur_color;
                }
            }
        }
    }
}

// -------------------------------------------------------------
// App 7: Notes.app Window
// -------------------------------------------------------------
static void draw_notes_content(window_t* win) {
    const ui_theme_t* theme = theme_get_current();
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    int sidebar_w = 120;
    gfx_draw_rect(cx, cy, sidebar_w, ch, theme->sidebar_bg);
    gfx_draw_line(cx + sidebar_w, cy, cx + sidebar_w, cy + ch, theme->sidebar_border);

    font_draw_text(cx + 10, cy + 10, "Quick Notes", theme->text_secondary, FONT_SIZE_SMALL);
    gfx_draw_rounded_rect(cx + 6, cy + 28, sidebar_w - 12, 32, 4, theme->accent);
    font_draw_text(cx + 12, cy + 34, "Architecture", theme->text_on_accent, FONT_SIZE_REGULAR);
    font_draw_text(cx + 12, cy + 48, "Sep 8, 2026", theme->text_on_accent, FONT_SIZE_SMALL);

    int rx = cx + sidebar_w + 16;
    gfx_draw_rect(cx + sidebar_w + 1, cy, cw - sidebar_w - 1, ch, theme->panel_bg);

    font_draw_text(rx, cy + 14, "RatanaOS Design System Notes", theme->text_primary, FONT_SIZE_LARGE);
    font_draw_text(rx, cy + 42, "1. Centralized Theme Engine with Light/Dark/Auto presets.", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(rx, cy + 62, "2. Multi-scale Typography Subsystem (Small, Regular, Large, Title).", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(rx, cy + 82, "3. Global Integer UI Scaling (80%, 100%, 125%, 150%).", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(rx, cy + 102, "4. Upstream WhiteSur & MacTahoe SVG Icon Suites.", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(rx, cy + 122, "5. Hardware 32-bit ARGB Linear Alpha Blending Compositor.", theme->text_primary, FONT_SIZE_REGULAR);
}

// -------------------------------------------------------------
// App 8: Music.app Window
// -------------------------------------------------------------
static void draw_music_content(window_t* win) {
    const ui_theme_t* theme = theme_get_current();
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    gfx_draw_rect(cx, cy, cw, ch, theme->panel_bg);

    // Album Art
    int ax = cx + 24;
    int ay = cy + 24;
    gfx_draw_rounded_rect(ax, ay, 120, 120, 10, 0x00FA2D48);
    icon_draw_music_48(ax + 36, ay + 36);

    // Track Metadata
    int mx = ax + 140;
    font_draw_text(mx, ay + 20, "RatanaOS Symphony in C", theme->text_primary, FONT_SIZE_LARGE);
    font_draw_text(mx, ay + 48, "Developer Soundtrack - 64-bit Long Mode", theme->text_secondary, FONT_SIZE_REGULAR);
    font_draw_text(mx, ay + 68, "High-Resolution Audio Engine (44.1 kHz)", theme->accent, FONT_SIZE_SMALL);

    // Progress Bar
    int bar_y = cy + 160;
    gfx_draw_rounded_rect(cx + 24, bar_y, cw - 48, 6, 3, theme->control_bg);
    gfx_draw_rounded_rect(cx + 24, bar_y, (cw - 48) / 3, 6, 3, 0x00FA2D48);
    font_draw_text(cx + 24, bar_y + 12, "1:24", theme->text_secondary, FONT_SIZE_SMALL);
    font_draw_text(cx + cw - 56, bar_y + 12, "3:58", theme->text_secondary, FONT_SIZE_SMALL);

    // Playback Controls
    int btn_cy = bar_y + 24;
    font_draw_text(cx + cw / 2 - 48, btn_cy, "|<<", theme->text_primary, FONT_SIZE_REGULAR);
    gfx_draw_circle(cx + cw / 2, btn_cy + 8, 16, 0x00FA2D48);
    font_draw_text(cx + cw / 2 - 4, btn_cy, "||", COLOR_WHITE, FONT_SIZE_REGULAR);
    font_draw_text(cx + cw / 2 + 36, btn_cy, ">>|", theme->text_primary, FONT_SIZE_REGULAR);
}

// -------------------------------------------------------------
// App 9: System Settings.app Window (520x350) - Appearance & UI Engine
// -------------------------------------------------------------
static int settings_active_tab = 0;

static void draw_settings_content(window_t* win) {
    const ui_theme_t* theme = theme_get_current();
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    int sidebar_w = 116;
    const char* sb_items[] = {
        "Appearance",
        "Themes",
        "Displays",
        "Typography",
        "Windows",
        "Dock",
        "Menu Bar",
        "Icons",
        "About"
    };
    ui_sidebar_draw(cx, cy, sidebar_w, ch, "Settings", sb_items, 9, settings_active_tab, -1);

    int rx = cx + sidebar_w + 12;
    int rw = cw - sidebar_w - 16;
    gfx_draw_rect(cx + sidebar_w + 1, cy, cw - sidebar_w - 1, ch, theme->panel_bg);

    if (settings_active_tab == 0) {
        // Tab 0: Appearance
        ui_label_draw_header(rx, cy + 8, "Appearance & Accent System");
        gfx_draw_line(rx, cy + 26, cx + cw - 16, cy + 26, theme->border);

        // Section 1: Mode (Light / Dark / Auto)
        font_draw_text(rx, cy + 32, "Interface Mode:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* mode_btns[] = {"Light", "Dark", "Auto (Day/Night)"};
        int cur_m = (int)theme_get_mode();
        ui_segmented_draw(rx, cy + 46, rw - 16, 22, mode_btns, 3, cur_m, -1);

        // Section 2: Accent Color Palette
        font_draw_text(rx, cy + 74, "Accent Color Palette:", theme->text_secondary, FONT_SIZE_SMALL);
        ui_accent_color_t cur_acc = theme_get_accent();
        for (int i = 0; i < ACCENT_COUNT; i++) {
            int ax = rx + i * 26 + 6;
            int ay = cy + 94;
            uint32_t col = theme_get_accent_color((ui_accent_color_t)i);
            gfx_draw_circle(ax, ay, 8, col);
            if (cur_acc == (ui_accent_color_t)i) {
                gfx_draw_circle(ax, ay, 10, theme->text_primary);
            }
        }

        // Section 3: Visual Effects Toggles
        font_draw_text(rx, cy + 114, "Translucency:", theme->text_primary, FONT_SIZE_REGULAR);
        ui_toggle_draw(rx + 100, cy + 112, theme->transparency_enabled, false);

        font_draw_text(rx + 160, cy + 114, "Drop Shadows:", theme->text_primary, FONT_SIZE_REGULAR);
        ui_toggle_draw(rx + 270, cy + 112, theme->shadows_enabled, false);

        // Section 4: Live Accent Swatch Preview
        font_draw_text(rx, cy + 144, "Interactive Accent Preview:", theme->text_secondary, FONT_SIZE_SMALL);
        ui_button_draw(rx, cy + 160, 110, 22, "Primary Action", UI_BUTTON_PRIMARY, UI_STATE_NORMAL);
        ui_button_draw(rx + 120, cy + 160, 100, 22, "Secondary", UI_BUTTON_SECONDARY, UI_STATE_NORMAL);
        ui_toggle_draw(rx + 230, cy + 160, true, false);

    } else if (settings_active_tab == 1) {
        // Tab 1: Themes (Profiles)
        ui_label_draw_header(rx, cy + 8, "Theme Presets & Profiles");
        gfx_draw_line(rx, cy + 26, cx + cw - 16, cy + 26, theme->border);

        font_draw_text(rx, cy + 32, "Active Preset:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* theme_btns[] = {"Dark", "Light", "macOS Dark", "macOS Light", "Auto"};
        int cur_p = (int)theme_get_preset();
        ui_segmented_draw(rx, cy + 46, rw - 16, 22, theme_btns, 5, cur_p, -1);

        font_draw_text(rx, cy + 76, "Dynamic Wallpaper Multi-tone Gradient:", theme->text_secondary, FONT_SIZE_SMALL);
        int grad_w = rw - 16;
        int grad_h = 32;
        int gy = cy + 92;
        gfx_draw_rounded_rect(rx, gy, grad_w, grad_h, 4, theme->wallpaper_top);
        gfx_draw_line(rx, gy + 10, rx + grad_w, gy + 10, theme->wallpaper_mid);
        gfx_draw_line(rx, gy + 20, rx + grad_w, gy + 20, theme->wallpaper_bot);
        gfx_draw_rounded_rect_outline(rx, gy, grad_w, grad_h, 4, theme->border);

        ui_button_draw(rx, cy + 136, 120, 22, "Cycle Preset", UI_BUTTON_PRIMARY, UI_STATE_NORMAL);
        font_draw_text(rx + 130, cy + 138, theme_get_preset_name(theme_get_preset()), theme->text_primary, FONT_SIZE_REGULAR);

    } else if (settings_active_tab == 2) {
        // Tab 2: Displays (UI Scale)
        ui_label_draw_header(rx, cy + 8, "Displays & UI Scaling");
        gfx_draw_line(rx, cy + 26, cx + cw - 16, cy + 26, theme->border);

        font_draw_text(rx, cy + 34, "Display Resolution: 1024 x 768 @ 60 Hz", theme->text_primary, FONT_SIZE_REGULAR);
        font_draw_text(rx, cy + 52, "Color Depth:        32-bit ARGB TrueColor", theme->text_secondary, FONT_SIZE_REGULAR);
        font_draw_text(rx, cy + 70, "Hardware Adapter:   BGA / VESA VBE Double Buffer", theme->text_secondary, FONT_SIZE_REGULAR);

        font_draw_text(rx, cy + 96, "Global UI Scaling Factor:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* scale_btns[] = {"80%", "100%", "125%", "150%"};
        int cur_sc = ui_scale_get();
        int sc_idx = (cur_sc <= 80) ? 0 : (cur_sc <= 100) ? 1 : (cur_sc <= 125) ? 2 : 3;
        ui_segmented_draw(rx, cy + 112, 200, 22, scale_btns, 4, sc_idx, -1);

        font_draw_text(rx, cy + 146, "Current Scale Factor:", theme->text_secondary, FONT_SIZE_SMALL);
        font_draw_text(rx + 130, cy + 144, ui_scale_get_label(), theme->accent, FONT_SIZE_TITLE);

    } else if (settings_active_tab == 3) {
        // Tab 3: Typography & Fonts
        ui_label_draw_header(rx, cy + 8, "Typography & Font Scaling");
        gfx_draw_line(rx, cy + 26, cx + cw - 16, cy + 26, theme->border);

        font_draw_text(rx, cy + 32, "Active Font Size Role:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* font_btns[] = {"Small (12)", "Regular (16)", "Large (24)", "Title (32)"};
        int cur_fs = (int)font_get_active_size();
        ui_segmented_draw(rx, cy + 46, rw - 16, 22, font_btns, 4, cur_fs, -1);

        font_draw_text(rx, cy + 76, "Typography Scale Factor:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* fscale_btns[] = {"80%", "100%", "120%", "140%"};
        int cur_fsc = font_get_scale();
        int fsc_idx = (cur_fsc <= 80) ? 0 : (cur_fsc <= 100) ? 1 : (cur_fsc <= 120) ? 2 : 3;
        ui_segmented_draw(rx, cy + 90, 200, 22, fscale_btns, 4, fsc_idx, -1);

        font_draw_text(rx, cy + 122, "Live Typography Hierarchy Preview:", theme->text_secondary, FONT_SIZE_SMALL);
        font_draw_text(rx, cy + 138, "Aa RatanaOS macOS Typography", theme->text_primary, font_get_active_size());

    } else if (settings_active_tab == 4) {
        // Tab 4: Windows (Window Style Engine)
        ui_label_draw_header(rx, cy + 8, "Window Style & Decorations");
        gfx_draw_line(rx, cy + 26, cx + cw - 16, cy + 26, theme->border);

        font_draw_text(rx, cy + 32, "Window Style Decoration:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* wstyle_btns[] = {"macOS", "Classic", "Minimal", "Transparent"};
        int cur_ws = (int)theme_get_window_style();
        ui_segmented_draw(rx, cy + 46, rw - 16, 22, wstyle_btns, 4, cur_ws, -1);

        font_draw_text(rx, cy + 76, "Corner Radius:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* rad_btns[] = {"0 px", "4 px", "8 px", "10 px", "14 px", "18 px"};
        int cur_rad = theme->window_radius;
        int rad_idx = (cur_rad <= 0) ? 0 : (cur_rad <= 4) ? 1 : (cur_rad <= 8) ? 2 : (cur_rad <= 10) ? 3 : (cur_rad <= 14) ? 4 : 5;
        ui_segmented_draw(rx, cy + 90, 210, 20, rad_btns, 6, rad_idx, -1);

        font_draw_text(rx + 220, cy + 76, "Shadow Size:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* sh_btns[] = {"None", "Small", "Medium", "Large"};
        int cur_sh = theme->window_shadow_size;
        int sh_idx = (cur_sh <= 0) ? 0 : (cur_sh <= 4) ? 1 : (cur_sh <= 6) ? 2 : 3;
        ui_segmented_draw(rx + 220, cy + 90, rw - 236, 20, sh_btns, 4, sh_idx, -1);

        // Live Window Preview
        font_draw_text(rx, cy + 118, "Live Window Style Preview:", theme->text_secondary, FONT_SIZE_SMALL);
        int pw_x = rx + 4;
        int pw_y = cy + 134;
        int pw_w = rw - 24;
        int pw_h = 50;
        int pr = theme->window_radius;
        if (theme->shadows_enabled) {
            gfx_draw_shadow(pw_x, pw_y, pw_w, pw_h, pr, theme->window_shadow_size, theme->shadow_alpha);
        }
        gfx_draw_rounded_rect(pw_x, pw_y, pw_w, pw_h, pr, theme->window_bg);
        gfx_draw_rounded_rect_outline(pw_x, pw_y, pw_w, pw_h, pr, theme->window_border_active);
        gfx_draw_rounded_rect(pw_x + 1, pw_y + 1, pw_w - 2, 18, (pr > 1 ? pr - 1 : 1), theme->window_titlebar);
        gfx_draw_circle(pw_x + 10, pw_y + 10, 4, COLOR_MAC_CLOSE);
        gfx_draw_circle(pw_x + 22, pw_y + 10, 4, COLOR_MAC_MIN);
        gfx_draw_circle(pw_x + 34, pw_y + 10, 4, COLOR_MAC_ZOOM);
        font_draw_text(pw_x + 50, pw_y + 4, "RatanaOS Preview Window", theme->window_title_active, FONT_SIZE_SMALL);

    } else if (settings_active_tab == 5) {
        // Tab 5: Dock (Dock Style Engine)
        ui_label_draw_header(rx, cy + 8, "Dock Position, Style & Behavior");
        gfx_draw_line(rx, cy + 26, cx + cw - 16, cy + 26, theme->border);

        font_draw_text(rx, cy + 32, "Dock Screen Edge Position:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* dpos_btns[] = {"Bottom", "Left Edge", "Right Edge"};
        int cur_dp = (int)dock_get_position();
        ui_segmented_draw(rx, cy + 46, rw - 16, 22, dpos_btns, 3, cur_dp, -1);

        font_draw_text(rx, cy + 74, "Dock Style Backing:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* dsty_btns[] = {"Frosted Glass", "Classic Solid", "Transparent", "Compact"};
        int cur_dst = (int)dock_get_style();
        ui_segmented_draw(rx, cy + 88, rw - 16, 22, dsty_btns, 4, cur_dst, -1);

        font_draw_text(rx, cy + 116, "Dock Icon Size:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* dsize_btns[] = {"32 px", "40 px", "48 px", "56 px", "64 px"};
        int cur_ds = dock_get_icon_size();
        int ds_idx = (cur_ds <= 32) ? 0 : (cur_ds <= 40) ? 1 : (cur_ds <= 48) ? 2 : (cur_ds <= 56) ? 3 : 4;
        ui_segmented_draw(rx, cy + 130, 180, 20, dsize_btns, 5, ds_idx, -1);

        font_draw_text(rx + 195, cy + 116, "Dock Spacing:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* sp_btns[] = {"4 px", "6 px", "8 px", "12 px"};
        int cur_sp = dock_get_spacing();
        int sp_idx = (cur_sp <= 4) ? 0 : (cur_sp <= 6) ? 1 : (cur_sp <= 8) ? 2 : 3;
        ui_segmented_draw(rx + 195, cy + 130, rw - 211, 20, sp_btns, 4, sp_idx, -1);

        font_draw_text(rx, cy + 158, "Magnification:", theme->text_primary, FONT_SIZE_REGULAR);
        ui_toggle_draw(rx + 95, cy + 156, dock_get_magnification(), false);

        font_draw_text(rx + 155, cy + 158, "Auto-Hide Dock:", theme->text_primary, FONT_SIZE_REGULAR);
        ui_toggle_draw(rx + 265, cy + 156, dock_get_autohide(), false);

    } else if (settings_active_tab == 6) {
        // Tab 6: Menu Bar
        ui_label_draw_header(rx, cy + 8, "Menu Bar Style & Behavior");
        gfx_draw_line(rx, cy + 26, cx + cw - 16, cy + 26, theme->border);

        font_draw_text(rx, cy + 32, "Top Status Bar Style:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* mstyle_btns[] = {"Frosted Glass", "Solid Opaque", "Transparent"};
        int cur_ms = (int)theme_get_menubar_style();
        ui_segmented_draw(rx, cy + 46, rw - 16, 22, mstyle_btns, 3, cur_ms, -1);

        font_draw_text(rx, cy + 80, "Live Menu Bar Strip Preview:", theme->text_secondary, FONT_SIZE_SMALL);
        int mb_y = cy + 96;
        int mb_w = rw - 16;
        uint32_t mb_bg = theme->dark_mode ? 0x001C1C1E : 0x00F0F0F4;
        gfx_draw_rect(rx, mb_y, mb_w, 24, mb_bg);
        gfx_draw_line(rx, mb_y + 24, rx + mb_w, mb_y + 24, theme->border);
        icon_draw_apple_logo(rx + 8, mb_y + 4, theme->text_primary);
        font_draw_text(rx + 28, mb_y + 4, "Finder  File  Edit  View  Help", theme->text_primary, FONT_SIZE_REGULAR);
        icon_draw_wifi(rx + mb_w - 40, mb_y + 8, theme->text_primary);
        icon_draw_battery(rx + mb_w - 20, mb_y + 8, theme->text_primary);

    } else if (settings_active_tab == 7) {
        // Tab 7: Icons
        ui_label_draw_header(rx, cy + 8, "Icon Themes & Desktop Customization");
        gfx_draw_line(rx, cy + 26, cx + cw - 16, cy + 26, theme->border);

        font_draw_text(rx, cy + 32, "Active Icon Suite:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* icon_theme_btns[] = {"WhiteSur", "MacTahoe", "Vector (Retro)"};
        int cur_ith = (int)icon_get_theme();
        ui_segmented_draw(rx, cy + 46, rw - 16, 22, icon_theme_btns, 3, cur_ith, -1);

        settings_t* s = settings_get();
        font_draw_text(rx, cy + 76, "Desktop Icons:", theme->text_primary, FONT_SIZE_REGULAR);
        ui_toggle_draw(rx + 95, cy + 74, s->show_desktop_icons, false);

        font_draw_text(rx + 145, cy + 76, "Desktop Size:", theme->text_secondary, FONT_SIZE_SMALL);
        const char* dt_sizes[] = {"32 px", "48 px", "64 px"};
        int dt_idx = (s->desktop_icon_size <= 32) ? 0 : (s->desktop_icon_size <= 48) ? 1 : 2;
        ui_segmented_draw(rx + 225, cy + 74, rw - 241, 20, dt_sizes, 3, dt_idx, -1);

        font_draw_text(rx, cy + 104, "Show Labels:", theme->text_primary, FONT_SIZE_REGULAR);
        ui_toggle_draw(rx + 95, cy + 102, s->show_icon_labels, false);

        // Scaled Live Icon Suite Preview
        font_draw_text(rx, cy + 132, "Scaled Icon Rasterizer Preview:", theme->text_secondary, FONT_SIZE_SMALL);
        int prev_y = cy + 148;
        int psize = 40;
        int step_px = psize + 10;
        icon_draw_scaled(ICON_ID_FINDER, rx + 4 + step_px * 0, prev_y, psize);
        icon_draw_scaled(ICON_ID_SAFARI, rx + 4 + step_px * 1, prev_y, psize);
        icon_draw_scaled(ICON_ID_TERMINAL, rx + 4 + step_px * 2, prev_y, psize);
        icon_draw_scaled(ICON_ID_SETTINGS, rx + 4 + step_px * 3, prev_y, psize);
        icon_draw_scaled(ICON_ID_DRIVE, rx + 4 + step_px * 4, prev_y, psize);
        icon_draw_scaled(ICON_ID_TRASH, rx + 4 + step_px * 5, prev_y, psize);

    } else if (settings_active_tab == 8) {
        // Tab 8: About
        ui_label_draw_header(rx, cy + 8, "About RatanaOS macOS Edition");
        gfx_draw_line(rx, cy + 26, cx + cw - 16, cy + 26, theme->border);

        font_draw_text(rx, cy + 34, "RatanaOS 64-bit Desktop Edition", theme->text_primary, FONT_SIZE_REGULAR);
        font_draw_text(rx, cy + 52, "Architecture: x86_64 Long Mode (4-Level PML4 Paging)", theme->text_secondary, FONT_SIZE_REGULAR);
        font_draw_text(rx, cy + 70, "Desktop:      macOS Sequoia Glass Theme Engine", theme->text_secondary, FONT_SIZE_REGULAR);
        font_draw_text(rx, cy + 88, "Icon Suite:   WhiteSur & MacTahoe 32-bit ARGB", theme->text_secondary, FONT_SIZE_REGULAR);
        font_draw_text(rx, cy + 106, "Storage:      Persistent VFS (/etc/ratana/settings.conf)", theme->text_secondary, FONT_SIZE_REGULAR);
        font_draw_text(rx, cy + 124, "Shell Tools:  theme, icons, settings, scale, font", theme->text_secondary, FONT_SIZE_REGULAR);
    }

    // Common Action Buttons at Bottom
    gfx_draw_line(rx, cy + ch - 34, cx + cw - 16, cy + ch - 34, theme->border);
    ui_button_draw(rx, cy + ch - 28, 100, 22, "Reset Defaults", UI_BUTTON_SECONDARY, UI_STATE_NORMAL);
    ui_button_draw(rx + 110, cy + ch - 28, 80, 22, "Save Store", UI_BUTTON_SECONDARY, UI_STATE_NORMAL);
    ui_button_draw(cx + cw - 90, cy + ch - 28, 70, 22, "Apply", UI_BUTTON_PRIMARY, UI_STATE_NORMAL);
}

static void handle_settings_click(window_t* win, int rel_x, int rel_y, int btn) {
    (void)win; (void)btn;
    int sidebar_w = 116;
    int cx = 2;
    int cy = TITLEBAR_HEIGHT + 1;
    int ch = win->height - TITLEBAR_HEIGHT - 3;
    int rx = 2 + sidebar_w + 12;
    int rw = win->width - 4 - sidebar_w - 16;

    // 0. Sidebar Navigation Clicks (9 tabs)
    int tab_hit = ui_sidebar_hit_test(cx, cy, sidebar_w, ch, 9, rel_x, rel_y);
    if (tab_hit >= 0 && tab_hit < 9) {
        settings_active_tab = tab_hit;
        return;
    }

    // Common Bottom Action Buttons
    if (ui_button_hit_test(rx, cy + ch - 28, 100, 22, rel_x, rel_y)) {
        settings_reset_defaults();
        return;
    }
    if (ui_button_hit_test(rx + 110, cy + ch - 28, 80, 22, rel_x, rel_y)) {
        settings_save();
        return;
    }
    if (ui_button_hit_test(win->width - 94, cy + ch - 28, 70, 22, rel_x, rel_y)) {
        settings_apply();
        return;
    }

    if (settings_active_tab == 0) {
        // Tab 0: Appearance
        int m_hit = ui_segmented_hit_test(rx, cy + 46, rw - 16, 22, 3, rel_x, rel_y);
        if (m_hit >= 0 && m_hit < THEME_MODE_COUNT) {
            theme_set_mode((theme_mode_t)m_hit);
            settings_get()->theme_mode = (theme_mode_t)m_hit;
            return;
        }
        for (int i = 0; i < ACCENT_COUNT; i++) {
            int ax = rx + i * 26 + 6;
            int ay = cy + 94;
            int dx = rel_x - ax;
            int dy = rel_y - ay;
            if (dx * dx + dy * dy <= 100) {
                theme_set_accent((ui_accent_color_t)i);
                settings_get()->accent = (ui_accent_color_t)i;
                return;
            }
        }
        if (ui_toggle_hit_test(rx + 100, cy + 112, rel_x, rel_y)) {
            const ui_theme_t* t = theme_get_current();
            theme_set_transparency(!t->transparency_enabled);
            settings_get()->transparency = theme_get_current()->transparency_enabled;
            return;
        }
        if (ui_toggle_hit_test(rx + 270, cy + 112, rel_x, rel_y)) {
            const ui_theme_t* t = theme_get_current();
            theme_set_shadows(!t->shadows_enabled);
            settings_get()->shadows = theme_get_current()->shadows_enabled;
            return;
        }

    } else if (settings_active_tab == 1) {
        // Tab 1: Themes
        int p_hit = ui_segmented_hit_test(rx, cy + 46, rw - 16, 22, 5, rel_x, rel_y);
        if (p_hit >= 0 && p_hit < UI_THEME_COUNT) {
            theme_set_preset((ui_theme_preset_t)p_hit);
            settings_get()->ui_theme = (ui_theme_preset_t)p_hit;
            return;
        }
        if (ui_button_hit_test(rx, cy + 136, 120, 22, rel_x, rel_y)) {
            theme_next();
            settings_get()->ui_theme = theme_get_preset();
            return;
        }

    } else if (settings_active_tab == 2) {
        // Tab 2: Displays (UI Scale)
        int sc_hit = ui_segmented_hit_test(rx, cy + 112, 200, 22, 4, rel_x, rel_y);
        if (sc_hit >= 0) {
            int scales[] = {80, 100, 125, 150};
            ui_scale_set(scales[sc_hit]);
            settings_get()->ui_scale = scales[sc_hit];
            dock_init();
            return;
        }

    } else if (settings_active_tab == 3) {
        // Tab 3: Typography
        int fs_hit = ui_segmented_hit_test(rx, cy + 46, rw - 16, 22, 4, rel_x, rel_y);
        if (fs_hit >= 0) {
            font_set_active_size((font_size_t)fs_hit);
            settings_get()->font_size = (font_size_t)fs_hit;
            return;
        }
        int fsc_hit = ui_segmented_hit_test(rx, cy + 90, 200, 22, 4, rel_x, rel_y);
        if (fsc_hit >= 0) {
            int fscales[] = {80, 100, 120, 140};
            font_set_scale(fscales[fsc_hit]);
            settings_get()->font_scale = fscales[fsc_hit];
            return;
        }

    } else if (settings_active_tab == 4) {
        // Tab 4: Windows
        int ws_hit = ui_segmented_hit_test(rx, cy + 46, rw - 16, 22, 4, rel_x, rel_y);
        if (ws_hit >= 0) {
            theme_set_window_style((window_style_t)ws_hit);
            settings_get()->window_style = (window_style_t)ws_hit;
            return;
        }
        int rad_hit = ui_segmented_hit_test(rx, cy + 90, 210, 20, 6, rel_x, rel_y);
        if (rad_hit >= 0) {
            int rads[] = {0, 4, 8, 10, 14, 18};
            theme_set_window_radius(rads[rad_hit]);
            settings_get()->window_radius = rads[rad_hit];
            return;
        }
        int sh_hit = ui_segmented_hit_test(rx + 220, cy + 90, rw - 236, 20, 4, rel_x, rel_y);
        if (sh_hit >= 0) {
            int shs[] = {0, 4, 6, 10};
            theme_set_window_shadow_size(shs[sh_hit]);
            settings_get()->window_shadow_size = shs[sh_hit];
            return;
        }

    } else if (settings_active_tab == 5) {
        // Tab 5: Dock
        int dp_hit = ui_segmented_hit_test(rx, cy + 46, rw - 16, 22, 3, rel_x, rel_y);
        if (dp_hit >= 0) {
            dock_set_position((dock_position_t)dp_hit);
            settings_get()->dock_position = (dock_position_t)dp_hit;
            return;
        }
        int dst_hit = ui_segmented_hit_test(rx, cy + 88, rw - 16, 22, 4, rel_x, rel_y);
        if (dst_hit >= 0) {
            dock_set_style((dock_style_t)dst_hit);
            settings_get()->dock_style = (dock_style_t)dst_hit;
            return;
        }
        int ds_hit = ui_segmented_hit_test(rx, cy + 130, 180, 20, 5, rel_x, rel_y);
        if (ds_hit >= 0) {
            int sizes[] = {32, 40, 48, 56, 64};
            dock_set_icon_size(sizes[ds_hit]);
            settings_get()->dock_icon_size = sizes[ds_hit];
            return;
        }
        int sp_hit = ui_segmented_hit_test(rx + 195, cy + 130, rw - 211, 20, 4, rel_x, rel_y);
        if (sp_hit >= 0) {
            int sps[] = {4, 6, 8, 12};
            dock_set_spacing(sps[sp_hit]);
            settings_get()->dock_spacing = sps[sp_hit];
            return;
        }
        if (ui_toggle_hit_test(rx + 95, cy + 156, rel_x, rel_y)) {
            bool mag = !dock_get_magnification();
            dock_set_magnification(mag);
            settings_get()->dock_magnification = mag;
            return;
        }
        if (ui_toggle_hit_test(rx + 265, cy + 156, rel_x, rel_y)) {
            bool ah = !dock_get_autohide();
            dock_set_autohide(ah);
            settings_get()->dock_autohide = ah;
            return;
        }

    } else if (settings_active_tab == 6) {
        // Tab 6: Menu Bar
        int ms_hit = ui_segmented_hit_test(rx, cy + 46, rw - 16, 22, 3, rel_x, rel_y);
        if (ms_hit >= 0) {
            theme_set_menubar_style((menubar_style_t)ms_hit);
            settings_get()->menubar_style = (menubar_style_t)ms_hit;
            return;
        }

    } else if (settings_active_tab == 7) {
        // Tab 7: Icons
        int ith_hit = ui_segmented_hit_test(rx, cy + 46, rw - 16, 22, 3, rel_x, rel_y);
        if (ith_hit >= 0 && ith_hit < ICON_THEME_COUNT) {
            icon_set_theme((icon_theme_id_t)ith_hit);
            settings_get()->icon_theme = (icon_theme_id_t)ith_hit;
            return;
        }
        if (ui_toggle_hit_test(rx + 95, cy + 74, rel_x, rel_y)) {
            settings_t* s = settings_get();
            s->show_desktop_icons = !s->show_desktop_icons;
            return;
        }
        int dt_hit = ui_segmented_hit_test(rx + 225, cy + 74, rw - 241, 20, 3, rel_x, rel_y);
        if (dt_hit >= 0) {
            int dts[] = {32, 48, 64};
            settings_get()->desktop_icon_size = dts[dt_hit];
            return;
        }
        if (ui_toggle_hit_test(rx + 95, cy + 102, rel_x, rel_y)) {
            settings_t* s = settings_get();
            s->show_icon_labels = !s->show_icon_labels;
            return;
        }
    }
}


// App 10: App Store.app Window (500x300)
// -------------------------------------------------------------
static void draw_appstore_content(window_t* win) {
    const ui_theme_t* theme = theme_get_current();
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    gfx_draw_rect(cx, cy, cw, ch, theme->panel_bg);

    font_draw_text(cx + 16, cy + 12, "Featured Applications for RatanaOS", theme->text_primary, FONT_SIZE_REGULAR);

    // 2 App Store Cards
    int card_w = (cw - 48) / 2;
    int c1_x = cx + 16;
    int cy_pos = cy + 36;
    gfx_draw_rounded_rect(c1_x, cy_pos, card_w, 160, 8, theme->dark_mode ? 0x00202028 : 0x00EAEAEF);
    gfx_draw_rounded_rect_outline(c1_x, cy_pos, card_w, 160, 8, theme->border);
    icon_draw_terminal_48(c1_x + 14, cy_pos + 14);
    font_draw_text(c1_x + 72, cy_pos + 16, "Xcode Pro", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(c1_x + 72, cy_pos + 36, "Developer Tools", theme->text_secondary, FONT_SIZE_SMALL);
    font_draw_text(c1_x + 14, cy_pos + 76, "Native 64-bit IDE & C Compiler", theme->text_primary, FONT_SIZE_SMALL);
    ui_button_draw(c1_x + 14, cy_pos + 116, 72, 24, "GET", UI_BUTTON_PRIMARY, UI_STATE_NORMAL);

    // Card 2: Logic Pro
    int c2_x = c1_x + card_w + 16;
    gfx_draw_rounded_rect(c2_x, cy_pos, card_w, 160, 8, theme->dark_mode ? 0x00202028 : 0x00EAEAEF);
    gfx_draw_rounded_rect_outline(c2_x, cy_pos, card_w, 160, 8, theme->border);
    icon_draw_music_48(c2_x + 14, cy_pos + 14);
    font_draw_text(c2_x + 72, cy_pos + 16, "Logic Pro", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(c2_x + 72, cy_pos + 36, "Music Production", theme->text_secondary, FONT_SIZE_SMALL);
    font_draw_text(c2_x + 14, cy_pos + 76, "Multi-track Audio Studio", theme->text_primary, FONT_SIZE_SMALL);
    ui_button_draw(c2_x + 14, cy_pos + 116, 92, 24, "INSTALLED", UI_BUTTON_SECONDARY, UI_STATE_NORMAL);
}

// -------------------------------------------------------------
// App 11: About This Mac Dialog Window (480x320)
// -------------------------------------------------------------
static void draw_about_content(window_t* win) {
    const ui_theme_t* theme = theme_get_current();
    int cx = win->x + 24;
    int cy = win->y + TITLEBAR_HEIGHT + 24;

    // Large 64x64  Apple Emblem
    icon_draw_apple_64(cx, cy + 10);

    // System Information Details
    int tx = cx + 86;
    font_draw_text(tx, cy + 4, "RatanaOS Sequoia (2026 64-bit Edition)", theme->text_primary, FONT_SIZE_REGULAR);
    font_draw_text(tx, cy + 22, "Version 15.4 (Build 2026.09)", theme->text_secondary, FONT_SIZE_SMALL);

    cpu_info_t cpu = cpuid_get_info();

    int ly = cy + 48;
    int line_h = 18;

    font_draw_text(tx, ly + line_h * 0, "Model:    MacBook Pro (x86_64 Long Mode)", theme->text_primary, FONT_SIZE_SMALL);

    char cpu_line[64];
    strcpy(cpu_line, "Chip:     ");
    strncat(cpu_line, cpu.brand, 32);
    font_draw_text(tx, ly + line_h * 1, cpu_line, theme->text_primary, FONT_SIZE_SMALL);

    char mem_line[48];
    strcpy(mem_line, "Memory:   256 MB RAM (16 MB Dynamic Heap)");
    font_draw_text(tx, ly + line_h * 2, mem_line, theme->text_primary, FONT_SIZE_SMALL);

    font_draw_text(tx, ly + line_h * 3, "Startup:  RatanaOS HD (1024 MB Boot Volume)", theme->text_primary, FONT_SIZE_SMALL);
    font_draw_text(tx, ly + line_h * 4, "Graphics: VBE 32-bit Linear Framebuffer (1024x768)", theme->text_primary, FONT_SIZE_SMALL);
    font_draw_text(tx, ly + line_h * 5, "Serial:   C02RTM2026X86", theme->text_secondary, FONT_SIZE_SMALL);

    // System Report Button
    ui_button_draw(tx, cy + 170, 140, 26, "System Report...", UI_BUTTON_SECONDARY, UI_STATE_NORMAL);
}

// Built-in Telegram demo. It is a local UI sample, not a network client.
static void draw_telegram_content(window_t* win) {
    const ui_theme_t* theme = theme_get_current();
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;
    int sidebar_w = 130;

    gfx_draw_rect(cx, cy, sidebar_w, ch, theme->sidebar_bg);
    font_draw_text(cx + 14, cy + 14, "Telegram", theme->text_primary, FONT_SIZE_LARGE);
    font_draw_text(cx + 14, cy + 48, "Saved Messages", theme->accent, FONT_SIZE_REGULAR);
    font_draw_text(cx + 14, cy + 72, "RatanaOS Team", theme->text_primary, FONT_SIZE_REGULAR);
    gfx_draw_rect(cx + sidebar_w, cy, cw - sidebar_w, ch, theme->panel_bg);

    int px = cx + sidebar_w + 18;
    font_draw_text(px, cy + 16, "Saved Messages", theme->text_primary, FONT_SIZE_LARGE);
    font_draw_text(px, cy + 42, "Telegram Demo — local-only preview", theme->text_secondary, FONT_SIZE_SMALL);
    gfx_draw_rounded_rect(px, cy + 76, cw - sidebar_w - 36, 44, 10, theme->control_bg);
    font_draw_text(px + 12, cy + 88, "Welcome to Telegram on RatanaOS.", theme->text_primary, FONT_SIZE_REGULAR);
    gfx_draw_rounded_rect(px + 64, cy + 132, cw - sidebar_w - 100, 44, 10, theme->accent);
    font_draw_text(px + 76, cy + 144, "This is a built-in demo app.", theme->text_on_accent, FONT_SIZE_REGULAR);
    gfx_draw_rounded_rect(px, cy + ch - 42, cw - sidebar_w - 36, 28, 10, theme->control_bg);
    font_draw_text(px + 10, cy + ch - 34, "Network messaging is not available yet", theme->text_disabled, FONT_SIZE_REGULAR);
}

// -------------------------------------------------------------
// §5. macOS Desktop Compositor & Event Handling
// -------------------------------------------------------------
static void draw_macos_desktop(void) {
    const ui_theme_t* theme = theme_get_current();
    int sw = gfx_get_width();

    // 1. Wallpaper Multi-tone Gradient
    gfx_draw_wallpaper();

    // 2. Desktop Icons on Right Side
    settings_t* s = settings_get();
    if (s->show_desktop_icons) {
        uint32_t dt_col = theme->dark_mode ? COLOR_WHITE : 0x001A1A1E;
        int d_size = (s->desktop_icon_size >= 32 && s->desktop_icon_size <= 64) ? s->desktop_icon_size : 48;
        int d_x = sw - d_size - 26;
        int step_y = d_size + (s->show_icon_labels ? 42 : 16);

        // 2a. Drive Icon (RatanaOS HD)
        icon_draw_scaled(ICON_ID_DRIVE, d_x, 40, d_size);
        if (s->show_icon_labels) {
            font_draw_text(sw - 88, 40 + d_size + 4, "RatanaOS HD", dt_col, FONT_SIZE_REGULAR);
        }

        // 2b. Applications Folder
        icon_draw_scaled(ICON_ID_FOLDER, d_x, 40 + step_y, d_size);
        if (s->show_icon_labels) {
            font_draw_text(sw - 94, 40 + step_y + d_size + 4, "Applications", dt_col, FONT_SIZE_REGULAR);
        }

        // 2c. Documents Folder
        icon_draw_scaled(ICON_ID_FOLDER, d_x, 40 + step_y * 2, d_size);
        if (s->show_icon_labels) {
            font_draw_text(sw - 86, 40 + step_y * 2 + d_size + 4, "Documents", dt_col, FONT_SIZE_REGULAR);
        }
    }

    // 3. Render Windows in Z-Order
    for (int i = 0; i < window_count; i++) {
        draw_window(&windows[i]);
    }

    // 4. Render Top Menu Bar
    const char* active_name = "Finder";
    for (int i = window_count - 1; i >= 0; i--) {
        if (windows[i].is_open && !windows[i].is_minimized && windows[i].is_active) {
            active_name = windows[i].title;
            break;
        }
    }
    menubar_draw(active_name);

    // 5. Render Floating Bottom Dock
    bool open_states[12];
    bool active_states[12];
    for (int i = 0; i < 12; i++) {
        window_t* dock_window = window_by_id(i);
        open_states[i] = dock_window && dock_window->is_open;
        active_states[i] = dock_window && dock_window->is_active;
    }
    dock_draw(open_states, active_states, window_count);
}

static void process_gui_events(void) {
    int mx = mouse_get_x();
    int my = mouse_get_y();
    dock_update_cursor(mx, my);
    bool left_click = mouse_is_left_clicked();
    bool click_down = left_click && !prev_left_click;
    int sw = gfx_get_width();

    // Check Window Dragging
    for (int i = window_count - 1; i >= 0; i--) {
        if (windows[i].is_dragging) {
            if (left_click) {
                windows[i].x = mx - windows[i].drag_offset_x;
                windows[i].y = my - windows[i].drag_offset_y;
                if (windows[i].x < 0) windows[i].x = 0;
                if (windows[i].y < MENUBAR_HEIGHT) windows[i].y = MENUBAR_HEIGHT;
            } else {
                windows[i].is_dragging = false;
            }
            break;
        }
    }

    if (click_down) {
        // 1. Menu Bar Hit Testing
        menubar_hit_t m_hit = menubar_hit_test(mx, my);
        if (m_hit == MENUBAR_HIT_APPLE_LOGO) {
            menubar_toggle_apple();
            prev_left_click = left_click;
            return;
        } else if (m_hit == MENUBAR_HIT_ITEM_ABOUT) {
            show_window(11);
            prev_left_click = left_click;
            return;
        } else if (m_hit == MENUBAR_HIT_ITEM_SETTINGS) {
            show_window(9);
            prev_left_click = left_click;
            return;
        } else if (m_hit == MENUBAR_HIT_ITEM_EXIT_CLI) {
            gui_exit();
            return;
        } else {
            menubar_close_apple();
        }

        // 2. Floating Dock Hit Testing
        int dock_idx = dock_hit_test(mx, my);
        if (dock_idx >= 0) {
            if (dock_idx >= 0 && dock_idx < 12 && dock_idx < window_count) {
                window_t* dock_window = window_by_id(dock_idx);
                if (dock_window) {
                    if (!dock_window->is_open) {
                        show_window(dock_idx);
                    } else if (dock_window->is_minimized) {
                        show_window(dock_idx);
                    } else if (dock_window->is_active) {
                        dock_window->is_minimized = true;
                    } else {
                        focus_window_by_id(dock_idx);
                    }
                }
            } else if (dock_idx == 12) { // Trash Icon
                memset(paint_canvas, 0, sizeof(paint_canvas));
            }
            prev_left_click = left_click;
            return;
        }

        // 3. Desktop Icons Click Testing
        // 3a. RatanaOS HD
        if (mx >= sw - 88 && mx <= sw - 20 && my >= 40 && my <= 110) {
            show_window(0); // Finder
            prev_left_click = left_click;
            return;
        }
        // 3b. Applications Folder
        if (mx >= sw - 94 && mx <= sw - 20 && my >= 130 && my <= 200) {
            show_window(1); // Launchpad
            prev_left_click = left_click;
            return;
        }
        // 3c. Documents Folder
        if (mx >= sw - 88 && mx <= sw - 20 && my >= 220 && my <= 290) {
            show_window(7); // Notes
            prev_left_click = left_click;
            return;
        }

        // 4. Window Hit Testing & Traffic Lights
        for (int i = window_count - 1; i >= 0; i--) {
            window_t* win = &windows[i];
            if (!win->is_open || win->is_minimized) continue;

            if (mx >= win->x && mx < win->x + win->width && my >= win->y && my < win->y + win->height) {
                int window_id = win->id;
                focus_window(i);
                win = window_by_id(window_id);
                if (!win) break;

                if (my < win->y + TITLEBAR_HEIGHT) {
                    titlebar_hit_t t_hit = window_hit_test_titlebar(win, mx, my);
                    if (t_hit == TITLEBAR_HIT_CLOSE) {
                        win->is_open = false;
                    } else if (t_hit == TITLEBAR_HIT_MINIMIZE) {
                        win->is_minimized = true;
                    } else if (t_hit == TITLEBAR_HIT_ZOOM) {
                        if (win->width < 600) {
                            win->width = 640;
                            win->height = 420;
                        } else {
                            win->width = win->orig_w;
                            win->height = win->orig_h;
                        }
                    } else if (t_hit == TITLEBAR_HIT_DRAG) {
                        win->is_dragging = true;
                        win->drag_offset_x = mx - win->x;
                        win->drag_offset_y = my - win->y;
                    }
                } else if (win->handle_click) {
                    win->handle_click(win, mx - win->x, my - win->y, 1);
                }
                break;
            }
        }
    }

    prev_left_click = left_click;
}

void gui_init(multiboot_info_t* mbi) {
    gfx_init(mbi);
    mouse_init();
    mouse_set_bounds(gfx_get_width(), gfx_get_height());

    font_init();
    theme_init();
    settings_init();
    menubar_init();
    dock_init();

    window_count = 0;
    memset(paint_canvas, 0, sizeof(paint_canvas));

    // Register all 12 macOS Desktop Applications
    create_window(0, "Finder", 40, 48, 480, 260, draw_finder_content, NULL);
    create_window(1, "Launchpad", 240, 100, 540, 360, draw_launchpad_content, NULL);
    create_window(2, "Safari", 220, 60, 560, 360, draw_safari_content, NULL);
    create_window(3, "Terminal", 540, 48, 460, 260, draw_terminal_content, NULL);
    create_window(4, "Activity Monitor", 540, 320, 460, 280, draw_sysmon_content, NULL);
    create_window(5, "Calculator", 40, 320, 240, 280, draw_calc_content, handle_calc_click);
    create_window(6, "Paint Studio", 300, 320, 360, 280, draw_paint_content, handle_paint_click);
    create_window(7, "Notes", 260, 120, 480, 280, draw_notes_content, NULL);
    create_window(8, "Music", 280, 140, 460, 260, draw_music_content, NULL);
    create_window(9, "System Settings", 250, 80, 520, 350, draw_settings_content, handle_settings_click);
    create_window(10, "App Store", 260, 110, 500, 300, draw_appstore_content, NULL);
    create_window(11, "About This Mac", 272, 160, 480, 320, draw_about_content, NULL);
    create_window(12, "Telegram Demo", 300, 130, 500, 320, draw_telegram_content, NULL);

    // Initial desktop state: Open Finder and System Settings
    window_t* finder = window_by_id(0);
    window_t* settings = window_by_id(9);
    if (finder) finder->is_open = true;
    if (settings) settings->is_open = true;
    focus_window_by_id(9);
}

void gui_start(void) {
    gui_running = true;

    while (gui_running) {
        if (keyboard_has_key()) {
            char k = keyboard_getchar();
            if (k == 27) { // ESC -> Exit to CLI
                gui_exit();
                break;
            } else if (keyboard_is_ctrl_pressed() && keyboard_is_alt_pressed() && (k == 't' || k == 'T')) {
                icon_theme_next();
            } else if (keyboard_is_ctrl_pressed() && keyboard_is_alt_pressed() && (k == 'd' || k == 'D')) {
                theme_toggle_dark();
            } else if (keyboard_is_ctrl_pressed() && keyboard_is_alt_pressed() && (k == '+' || k == '=')) {
                ui_scale_up();
                dock_init();
            } else if (keyboard_is_ctrl_pressed() && keyboard_is_alt_pressed() && (k == '-' || k == '_')) {
                ui_scale_down();
                dock_init();
            } else if (keyboard_is_ctrl_pressed() && keyboard_is_alt_pressed() && (k == 's' || k == 'S')) {
                show_window(9);
            } else if (k == 't' || k == 'T') {
                icon_theme_next();
            } else if (k == 'd' || k == 'D') {
                theme_toggle_dark();
            } else if (k == '+' || k == '=') {
                ui_scale_increase();
                dock_init();
            } else if (k == '-' || k == '_') {
                ui_scale_decrease();
                dock_init();
            }
        }

        process_gui_events();
        draw_macos_desktop();
        mouse_draw_cursor(mouse_get_x(), mouse_get_y());
        gfx_swap_buffers();

        timer_sleep_ms(16);
    }
}

void gui_exit(void) {
    gui_running = false;
    vga_init();
    vga_clear();
    vga_enable_cursor(14, 15);
}

bool gui_is_running(void) {
    return gui_running;
}

bool gui_launch_app(const char* app_id) {
    if (app_id && strcmp(app_id, "telegram") == 0 && window_by_id(12)) {
        show_window(12);
        return true;
    }
    return false;
}
