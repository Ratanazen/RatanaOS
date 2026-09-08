#include "../include/gui.h"
#include "../include/gfx.h"
#include "../include/icons.h"
#include "../include/menubar.h"
#include "../include/dock.h"
#include "../include/mouse.h"
#include "../include/keyboard.h"
#include "../include/rtc.h"
#include "../include/heap.h"
#include "../include/pmm.h"
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

static void handle_calc_click(window_t* win, int rel_x, int rel_y, int btn);
static void handle_paint_click(window_t* win, int rel_x, int rel_y, int btn);

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

    // Window Drop Shadow (Alpha Blended)
    gfx_draw_rounded_rect_alpha(win->x + 8, win->y + 8, win->width, win->height, 10, 0x00050508, 160);

    // Main Window Frame (#1E1E24)
    gfx_draw_rounded_rect(win->x, win->y, win->width, win->height, 10, 0x001E1E24);
    gfx_draw_rounded_rect_outline(win->x, win->y, win->width, win->height, 10, win->is_active ? 0x0050505A : 0x0032323A);

    // macOS Titlebar (#2B2B2E)
    gfx_draw_rounded_rect(win->x + 1, win->y + 1, win->width - 2, TITLEBAR_HEIGHT, 9, 0x002B2B2E);
    gfx_draw_line(win->x, win->y + TITLEBAR_HEIGHT, win->x + win->width - 1, win->y + TITLEBAR_HEIGHT, 0x003A3A3C);

    // 🔴 🟡 🟢 Traffic Lights (12px Diameter)
    gfx_draw_circle(win->x + 14, win->y + 14, 6, COLOR_MAC_CLOSE);
    gfx_draw_circle(win->x + 32, win->y + 14, 6, COLOR_MAC_MIN);
    gfx_draw_circle(win->x + 50, win->y + 14, 6, COLOR_MAC_ZOOM);

    // Centered Window Title Text
    int title_px = (int)strlen(win->title) * 8;
    int title_x = win->x + (win->width - title_px) / 2;
    gfx_draw_string_transparent(title_x, win->y + 7, win->title, win->is_active ? COLOR_WHITE : 0x008E8E93);

    // Application Content
    if (win->draw_content) {
        win->draw_content(win);
    }
}

// -------------------------------------------------------------
// App 0: Finder Window
// -------------------------------------------------------------
static void draw_finder_content(window_t* win) {
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    int sidebar_w = 124;
    gfx_draw_rect(cx, cy, sidebar_w, ch, 0x00242428);
    gfx_draw_line(cx + sidebar_w, cy, cx + sidebar_w, cy + ch, 0x003A3A3C);

    gfx_draw_string_transparent(cx + 10, cy + 10, "Favorites", 0x008E8E93);
    gfx_draw_string_transparent(cx + 14, cy + 30, "> Applications", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 50, "> Desktop", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 70, "> Documents", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 90, "> Downloads", COLOR_WHITE);

    gfx_draw_string_transparent(cx + 10, cy + 120, "Locations", 0x008E8E93);
    gfx_draw_string_transparent(cx + 14, cy + 140, "[=] RatanaOS HD", 0x000A84FF);

    int rx = cx + sidebar_w + 14;
    gfx_draw_rect(cx + sidebar_w + 1, cy, cw - sidebar_w - 1, ch, 0x001C1C1E);

    // Row 1 of Applications
    icon_draw_safari_48(rx, cy + 14);
    gfx_draw_string_transparent(rx + 2, cy + 54, "Safari", COLOR_WHITE);

    icon_draw_terminal_48(rx + 80, cy + 14);
    gfx_draw_string_transparent(rx + 74, cy + 54, "Terminal", COLOR_WHITE);

    icon_draw_sysmon_48(rx + 160, cy + 14);
    gfx_draw_string_transparent(rx + 154, cy + 54, "Activity", COLOR_WHITE);

    icon_draw_calculator_48(rx + 240, cy + 14);
    gfx_draw_string_transparent(rx + 234, cy + 54, "Calculator", COLOR_WHITE);

    // Row 2 of Applications
    icon_draw_paint_48(rx, cy + 84);
    gfx_draw_string_transparent(rx + 6, cy + 124, "Paint", COLOR_WHITE);

    icon_draw_notes_48(rx + 80, cy + 84);
    gfx_draw_string_transparent(rx + 84, cy + 124, "Notes", COLOR_WHITE);

    icon_draw_music_48(rx + 160, cy + 84);
    gfx_draw_string_transparent(rx + 164, cy + 124, "Music", COLOR_WHITE);

    icon_draw_settings_48(rx + 240, cy + 84);
    gfx_draw_string_transparent(rx + 236, cy + 124, "Settings", COLOR_WHITE);

    gfx_draw_line(cx + sidebar_w, cy + ch - 24, cx + cw, cy + ch - 24, 0x003A3A3C);
    gfx_draw_string_transparent(rx, cy + ch - 18, "8 items, 256 MB available on RatanaOS HD", 0x008E8E93);
}

// -------------------------------------------------------------
// App 1: Launchpad Modal Window (520x360)
// -------------------------------------------------------------
static void draw_launchpad_content(window_t* win) {
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    gfx_draw_rect(cx, cy, cw, ch, 0x00141418);

    // Search Bar Pill
    gfx_draw_rounded_rect(cx + 120, cy + 10, cw - 240, 24, 12, 0x00282830);
    gfx_draw_rounded_rect_outline(cx + 120, cy + 10, cw - 240, 24, 12, 0x00484854);
    icon_draw_search(cx + 130, cy + 14, 0x008E8E93);
    gfx_draw_string_transparent(cx + 150, cy + 14, "Search Applications...", 0x008E8E93);

    // 4x3 Grid of App Icons
    int grid_x = cx + 36;
    int grid_y = cy + 46;
    int step_x = 112;
    int step_y = 80;

    // Row 0
    icon_draw_finder_48(grid_x + step_x * 0, grid_y + step_y * 0);
    gfx_draw_string_transparent(grid_x + step_x * 0 + 2, grid_y + step_y * 0 + 52, "Finder", COLOR_WHITE);

    icon_draw_safari_48(grid_x + step_x * 1, grid_y + step_y * 0);
    gfx_draw_string_transparent(grid_x + step_x * 1 + 2, grid_y + step_y * 0 + 52, "Safari", COLOR_WHITE);

    icon_draw_terminal_48(grid_x + step_x * 2, grid_y + step_y * 0);
    gfx_draw_string_transparent(grid_x + step_x * 2 - 4, grid_y + step_y * 0 + 52, "Terminal", COLOR_WHITE);

    icon_draw_sysmon_48(grid_x + step_x * 3, grid_y + step_y * 0);
    gfx_draw_string_transparent(grid_x + step_x * 3 - 6, grid_y + step_y * 0 + 52, "Activity", COLOR_WHITE);

    // Row 1
    icon_draw_calculator_48(grid_x + step_x * 0, grid_y + step_y * 1);
    gfx_draw_string_transparent(grid_x + step_x * 0 - 6, grid_y + step_y * 1 + 52, "Calculator", COLOR_WHITE);

    icon_draw_paint_48(grid_x + step_x * 1, grid_y + step_y * 1);
    gfx_draw_string_transparent(grid_x + step_x * 1 + 6, grid_y + step_y * 1 + 52, "Paint", COLOR_WHITE);

    icon_draw_notes_48(grid_x + step_x * 2, grid_y + step_y * 1);
    gfx_draw_string_transparent(grid_x + step_x * 2 + 6, grid_y + step_y * 1 + 52, "Notes", COLOR_WHITE);

    icon_draw_music_48(grid_x + step_x * 3, grid_y + step_y * 1);
    gfx_draw_string_transparent(grid_x + step_x * 3 + 6, grid_y + step_y * 1 + 52, "Music", COLOR_WHITE);

    // Row 2
    icon_draw_settings_48(grid_x + step_x * 0, grid_y + step_y * 2);
    gfx_draw_string_transparent(grid_x + step_x * 0 - 2, grid_y + step_y * 2 + 52, "Settings", COLOR_WHITE);

    icon_draw_appstore_48(grid_x + step_x * 1, grid_y + step_y * 2);
    gfx_draw_string_transparent(grid_x + step_x * 1 - 4, grid_y + step_y * 2 + 52, "App Store", COLOR_WHITE);

    icon_draw_about_48(grid_x + step_x * 2, grid_y + step_y * 2);
    gfx_draw_string_transparent(grid_x + step_x * 2 + 2, grid_y + step_y * 2 + 52, "About", COLOR_WHITE);

    icon_draw_folder_48(grid_x + step_x * 3, grid_y + step_y * 2);
    gfx_draw_string_transparent(grid_x + step_x * 3 + 2, grid_y + step_y * 2 + 52, "Folder", COLOR_WHITE);
}

// -------------------------------------------------------------
// App 2: Safari Browser Window (520x340)
// -------------------------------------------------------------
static void draw_safari_content(window_t* win) {
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    gfx_draw_rect(cx, cy, cw, ch, 0x001A1A1E);

    // Safari Toolbar (Navigation & Unified Smart Search Bar)
    gfx_draw_rect(cx, cy, cw, 34, 0x00242428);
    gfx_draw_line(cx, cy + 34, cx + cw, cy + 34, 0x003A3A3C);

    gfx_draw_string_transparent(cx + 10, cy + 9, "<  >", 0x008E8E93);

    // Unified URL Pill
    int url_w = cw - 120;
    gfx_draw_rounded_rect(cx + 60, cy + 5, url_w, 24, 12, 0x0018181C);
    gfx_draw_rounded_rect_outline(cx + 60, cy + 5, url_w, 24, 12, 0x003A3A3C);
    gfx_draw_string_transparent(cx + 74, cy + 9, "[*] https://apple.com/macos/sequoia", COLOR_WHITE);

    // Start Page Content
    gfx_draw_string_transparent(cx + 20, cy + 50, "Favorites", 0x008E8E93);

    // Favorite Cards
    static const char* fav_titles[4] = {"Apple", "RatanaOS Wiki", "GitHub", "Developer"};
    for (int i = 0; i < 4; i++) {
        int fx = cx + 20 + i * 118;
        int fy = cy + 74;
        gfx_draw_rounded_rect(fx, fy, 104, 70, 8, 0x00282830);
        gfx_draw_rounded_rect_outline(fx, fy, 104, 70, 8, 0x003E3E48);
        icon_draw_apple_logo(fx + 44, fy + 16, (i == 0) ? COLOR_WHITE : (i == 1 ? 0x000A84FF : 0x0030D158));
        gfx_draw_string_transparent(fx + 12, fy + 48, fav_titles[i], COLOR_WHITE);
    }

    // Hero Privacy Banner
    int bx = cx + 20;
    int by = cy + 164;
    gfx_draw_rounded_rect(bx, by, cw - 40, 84, 8, 0x00202026);
    gfx_draw_rounded_rect_outline(bx, by, cw - 40, 84, 8, 0x003A3A44);
    gfx_draw_string_transparent(bx + 16, by + 12, "Safari Privacy Report", 0x000A84FF);
    gfx_draw_string_transparent(bx + 16, by + 34, "RatanaOS Native 64-bit Engine active.", COLOR_WHITE);
    gfx_draw_string_transparent(bx + 16, by + 54, "100% Secure Isolated Kernel Sandbox.", 0x0030D158);
}

// -------------------------------------------------------------
// App 3: Terminal.app Window
// -------------------------------------------------------------
static void draw_terminal_content(window_t* win) {
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    gfx_draw_rect(cx, cy, cw, ch, 0x00101012);

    gfx_draw_string_transparent(cx + 10, cy + 8, "Last login: Tue Sep 8 22:33:00 on ttys000", 0x008E8E93);
    gfx_draw_string_transparent(cx + 10, cy + 28, "RatanaOS 64-bit (x86_64 Long Mode) Darwin 24.0.0", COLOR_GREEN);
    gfx_draw_string_transparent(cx + 10, cy + 52, "reny@macbook-pro ~ %", 0x000A84FF);
    gfx_draw_string_transparent(cx + 186, cy + 52, "neofetch", COLOR_WHITE);

    icon_draw_apple_logo(cx + 16, cy + 80, COLOR_GREEN);
    gfx_draw_string_transparent(cx + 42, cy + 76, "reny@RatanaOS-MacBookPro", 0x000A84FF);
    gfx_draw_string_transparent(cx + 42, cy + 94, "------------------------", 0x003A3A3C);
    gfx_draw_string_transparent(cx + 42, cy + 112, "OS:      RatanaOS Sequoia 15.4 (64-bit)", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 42, cy + 130, "Host:    MacBookPro18,1 (x86_64)", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 42, cy + 148, "Kernel:  64-bit Long Mode Kernel", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 42, cy + 166, "Shell:   zsh 5.9 (RatanaSH)", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 42, cy + 184, "DE:      Aqua / RatanaWM 64-bit", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 42, cy + 202, "Memory:  256MB / 16MB Heap", COLOR_WHITE);

    gfx_draw_string_transparent(cx + 10, cy + 226, "reny@macbook-pro ~ %", 0x000A84FF);
    uint32_t ticks = timer_get_ticks();
    if ((ticks / 50) % 2 == 0) {
        gfx_draw_rect(cx + 186, cy + 226, 8, 16, COLOR_WHITE);
    }
}

// -------------------------------------------------------------
// App 4: Activity Monitor.app Window
// -------------------------------------------------------------
static void draw_sysmon_content(window_t* win) {
    int cx = win->x + 8;
    int cy = win->y + TITLEBAR_HEIGHT + 8;

    heap_stats_t hstats = heap_get_stats();
    size_t free_pgs = pmm_get_free_pages();
    size_t total_pgs = pmm_get_total_pages();
    cpu_info_t cpu = cpuid_get_info();
    uint32_t uptime_sec = timer_get_ticks() / 100;

    gfx_draw_string_transparent(cx + 8, cy + 6, "Activity Monitor - CPU & Memory Pressure", 0x000A84FF);

    char cpu_str[64];
    strcpy(cpu_str, "Chip: ");
    strcat(cpu_str, cpu.brand[0] ? cpu.brand : "x86_64 Processor");
    gfx_draw_string_transparent(cx + 8, cy + 28, cpu_str, COLOR_WHITE);

    char upt_str[32];
    strcpy(upt_str, "System Uptime: ");
    char num_buf[16];
    utoa(uptime_sec, num_buf, 10);
    strcat(upt_str, num_buf);
    strcat(upt_str, " s");
    gfx_draw_string_transparent(cx + 8, cy + 46, upt_str, 0x008E8E93);

    gfx_draw_string_transparent(cx + 8, cy + 72, "Physical Memory (256 MB Unified)", COLOR_WHITE);
    int bar_w = win->width - 32;
    int used_pgs = (int)(total_pgs - free_pgs);
    int pmm_fill = total_pgs > 0 ? (used_pgs * bar_w) / (int)total_pgs : 0;
    gfx_draw_rect(cx + 8, cy + 90, bar_w, 16, 0x002C2C2E);
    gfx_draw_rect(cx + 8, cy + 90, pmm_fill < 12 ? 12 : pmm_fill, 16, COLOR_GREEN);
    gfx_draw_rect_outline(cx + 8, cy + 90, bar_w, 16, 0x003A3A3C);

    gfx_draw_string_transparent(cx + 8, cy + 118, "Memory Used (16384 KB Dynamic Heap)", COLOR_WHITE);
    int heap_fill = hstats.total_size > 0 ? (int)((hstats.used_size * bar_w) / hstats.total_size) : 0;
    gfx_draw_rect(cx + 8, cy + 136, bar_w, 16, 0x002C2C2E);
    gfx_draw_rect(cx + 8, cy + 136, heap_fill < 12 ? 12 : heap_fill, 16, COLOR_ORANGE);
    gfx_draw_rect_outline(cx + 8, cy + 136, bar_w, 16, 0x003A3A3C);

    gfx_draw_rect(cx + 8, cy + 162, bar_w, 80, 0x001C1C1E);
    gfx_draw_rect_outline(cx + 8, cy + 162, bar_w, 80, 0x003A3A3C);
    gfx_draw_string_transparent(cx + 14, cy + 168, "Process Name       % CPU    Memory    Threads", 0x008E8E93);
    gfx_draw_line(cx + 8, cy + 184, cx + 8 + bar_w, cy + 184, 0x003A3A3C);
    gfx_draw_string_transparent(cx + 14, cy + 190, "kernel_task         1.2%     24 MB       8", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 206, "WindowServer        3.4%     16 MB       4", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 222, "Finder              0.2%      8 MB       2", COLOR_WHITE);
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
    int cx = win->x + 12;
    int cy = win->y + TITLEBAR_HEIGHT + 10;

    gfx_draw_rect(cx, cy, 216, 42, 0x001C1C1E);
    gfx_draw_string_transparent(cx + 190 - (int)strlen(calc_display) * 8, cy + 14, calc_display, COLOR_WHITE);

    int btn_w = 48;
    int btn_h = 34;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int bx = cx + c * (btn_w + 8);
            int by = cy + 50 + r * (btn_h + 8);
            uint32_t bg = (c == 3) ? COLOR_ORANGE : (r == 0 ? 0x00505052 : 0x003A3A3C);
            gfx_draw_rounded_rect(bx, by, btn_w, btn_h, 6, bg);
            gfx_draw_rounded_rect_outline(bx, by, btn_w, btn_h, 6, 0x002C2C2E);
            gfx_draw_string_transparent(bx + 18, by + 10, calc_buttons[r][c], COLOR_WHITE);
        }
    }

    int by = cy + 50 + 4 * (btn_h + 8);
    gfx_draw_rounded_rect(cx, by, 104, btn_h, 6, 0x003A3A3C);
    gfx_draw_string_transparent(cx + 46, by + 10, "0", COLOR_WHITE);

    gfx_draw_rounded_rect(cx + 112, by, btn_w, btn_h, 6, 0x003A3A3C);
    gfx_draw_string_transparent(cx + 130, by + 10, ".", COLOR_WHITE);

    gfx_draw_rounded_rect(cx + 168, by, btn_w, btn_h, 6, COLOR_ORANGE);
    gfx_draw_string_transparent(cx + 186, by + 10, "=", COLOR_WHITE);
}

static void handle_calc_click(window_t* win, int rel_x, int rel_y, int btn) {
    (void)win;
    (void)btn;
    int cx = 12;
    int cy = TITLEBAR_HEIGHT + 10 + 50;
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
        } else if (rel_x >= cx + 168 && rel_x < cx + 168 + btn_w) {
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
            gfx_draw_pixel(cx + x, can_y + y, col != 0 ? col : COLOR_WHITE);
        }
    }
}

static void handle_paint_click(window_t* win, int rel_x, int rel_y, int btn) {
    (void)win;
    (void)btn;
    int cx = 8;
    int cy = TITLEBAR_HEIGHT + 8;

    if (rel_y >= cy && rel_y < cy + 22) {
        for (int i = 0; i < 8; i++) {
            int px = cx + i * 28;
            if (rel_x >= px && rel_x < px + 22) {
                paint_cur_color = paint_colors[i];
                return;
            }
        }
    }

    int can_y = cy + 30;
    if (rel_x >= cx && rel_x < cx + CANVAS_W && rel_y >= can_y && rel_y < can_y + CANVAS_H) {
        int px = rel_x - cx;
        int py = rel_y - can_y;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int nx = px + dx;
                int ny = py + dy;
                if (nx >= 0 && nx < CANVAS_W && ny >= 0 && ny < CANVAS_H) {
                    paint_canvas[ny * CANVAS_W + nx] = paint_cur_color;
                }
            }
        }
    }
}

// -------------------------------------------------------------
// App 7: Notes.app Window (460x280)
// -------------------------------------------------------------
static void draw_notes_content(window_t* win) {
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    int sidebar_w = 140;
    gfx_draw_rect(cx, cy, sidebar_w, ch, 0x00242428);
    gfx_draw_line(cx + sidebar_w, cy, cx + sidebar_w, cy + ch, 0x003A3A3C);

    gfx_draw_string_transparent(cx + 10, cy + 10, "All iCloud Notes", 0x00FFD60A);
    gfx_draw_rounded_rect(cx + 6, cy + 30, sidebar_w - 12, 36, 4, 0x003A3A44);
    gfx_draw_string_transparent(cx + 12, cy + 34, "RatanaOS Plan", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 12, cy + 50, "macOS Icon Clone...", 0x008E8E93);

    gfx_draw_string_transparent(cx + 12, cy + 76, "64-bit Architecture", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 12, cy + 92, "Long Mode Kernel...", 0x008E8E93);

    // Right legal notepad pane
    int rx = cx + sidebar_w + 1;
    int rw = cw - sidebar_w - 1;
    gfx_draw_rect(rx, cy, rw, ch, 0x001E1E22);

    gfx_draw_string_transparent(rx + 16, cy + 16, "RatanaOS 64-bit & macOS Sequoia Edition", 0x00FFD60A);
    gfx_draw_line(rx + 16, cy + 36, rx + rw - 16, cy + 36, 0x003A3A3C);

    gfx_draw_string_transparent(rx + 16, cy + 48, "- x86_64 Long Mode Kernel identity-mapped", COLOR_WHITE);
    gfx_draw_string_transparent(rx + 16, cy + 68, "- 16-byte 64-bit Interrupt Descriptor Table", COLOR_WHITE);
    gfx_draw_string_transparent(rx + 16, cy + 88, "- High-Definition Vector macOS Icon Suite", COLOR_WHITE);
    gfx_draw_string_transparent(rx + 16, cy + 108, "- Translucent Alpha-Blended Floating Dock", COLOR_WHITE);
    gfx_draw_string_transparent(rx + 16, cy + 128, "- 256MB RAM / 16MB Dynamic Heap Allocator", COLOR_WHITE);
}

// -------------------------------------------------------------
// App 8: Music.app Window (440x260)
// -------------------------------------------------------------
static void draw_music_content(window_t* win) {
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    gfx_draw_rect(cx, cy, cw, ch, 0x0018181C);

    // Album Art Card
    int art_x = cx + 20;
    int art_y = cy + 24;
    icon_draw_music_48(art_x, art_y);
    gfx_draw_rounded_rect(art_x, art_y, 72, 72, 8, 0x00FA2D48);
    gfx_draw_string_transparent(art_x + 18, art_y + 26, "♫", COLOR_WHITE);

    // Track Info
    int tx = art_x + 88;
    gfx_draw_string_transparent(tx, cy + 24, "Sequoia Symphony (64-bit Suite)", COLOR_WHITE);
    gfx_draw_string_transparent(tx, cy + 46, "RatanaOS Orchestra - Studio Master", 0x00FA2D48);
    gfx_draw_string_transparent(tx, cy + 68, "Lossless Audio * 24-bit / 192 kHz", 0x008E8E93);

    // Playback Timeline Scrubber
    int scrob_y = cy + 116;
    gfx_draw_rounded_rect(cx + 20, scrob_y, cw - 40, 6, 3, 0x003A3A40);
    gfx_draw_rounded_rect(cx + 20, scrob_y, (cw - 40) * 3 / 5, 6, 3, 0x00FA2D48); // 60% progress
    gfx_draw_circle(cx + 20 + (cw - 40) * 3 / 5, scrob_y + 3, 5, COLOR_WHITE);

    gfx_draw_string_transparent(cx + 20, scrob_y + 12, "02:14", 0x008E8E93);
    gfx_draw_string_transparent(cx + cw - 60, scrob_y + 12, "-01:42", 0x008E8E93);

    // Controls: Previous, Play/Pause, Next
    int btn_cy = scrob_y + 40;
    gfx_draw_string_transparent(cx + cw / 2 - 48, btn_cy, "|<<", COLOR_WHITE);
    gfx_draw_circle(cx + cw / 2, btn_cy + 8, 16, 0x00FA2D48);
    gfx_draw_string_transparent(cx + cw / 2 - 4, btn_cy, "||", COLOR_WHITE);
    gfx_draw_string_transparent(cx + cw / 2 + 36, btn_cy, ">>|", COLOR_WHITE);
}

// -------------------------------------------------------------
// App 9: System Settings.app Window (480x300)
// -------------------------------------------------------------
static void draw_settings_content(window_t* win) {
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    int sidebar_w = 130;
    gfx_draw_rect(cx, cy, sidebar_w, ch, 0x00242428);
    gfx_draw_line(cx + sidebar_w, cy, cx + sidebar_w, cy + ch, 0x003A3A3C);

    gfx_draw_string_transparent(cx + 10, cy + 10, "Settings", 0x008E8E93);
    gfx_draw_string_transparent(cx + 14, cy + 30, "[*] Wi-Fi", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 50, "[*] Bluetooth", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 70, "[*] Displays", COLOR_WHITE);
    gfx_draw_rounded_rect(cx + 6, cy + 90, sidebar_w - 12, 22, 4, 0x000A84FF);
    gfx_draw_string_transparent(cx + 14, cy + 94, "> General", COLOR_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 120, "[*] Wallpaper", COLOR_WHITE);

    int rx = cx + sidebar_w + 16;
    gfx_draw_rect(cx + sidebar_w + 1, cy, cw - sidebar_w - 1, ch, 0x001C1C1E);

    gfx_draw_string_transparent(rx, cy + 14, "General Settings", COLOR_WHITE);
    gfx_draw_line(rx, cy + 34, cx + cw - 16, cy + 34, 0x003A3A3C);

    gfx_draw_string_transparent(rx, cy + 46, "Compositor 60 FPS Engine:", COLOR_WHITE);
    gfx_draw_rounded_rect(rx + 230, cy + 44, 36, 18, 9, 0x0030D158);
    gfx_draw_circle(rx + 256, cy + 53, 7, COLOR_WHITE);

    gfx_draw_string_transparent(rx, cy + 74, "High-Res Vector Icon Suite:", COLOR_WHITE);
    gfx_draw_rounded_rect(rx + 230, cy + 72, 36, 18, 9, 0x0030D158);
    gfx_draw_circle(rx + 256, cy + 81, 7, COLOR_WHITE);

    gfx_draw_string_transparent(rx, cy + 102, "Dark Mode Aqua Theme:", COLOR_WHITE);
    gfx_draw_rounded_rect(rx + 230, cy + 100, 36, 18, 9, 0x0030D158);
    gfx_draw_circle(rx + 256, cy + 109, 7, COLOR_WHITE);

    gfx_draw_string_transparent(rx, cy + 130, "Virtual Machine Acceleration:", COLOR_WHITE);
    gfx_draw_rounded_rect(rx + 230, cy + 128, 36, 18, 9, 0x0030D158);
    gfx_draw_circle(rx + 256, cy + 137, 7, COLOR_WHITE);
}

// -------------------------------------------------------------
// App 10: App Store.app Window (480x280)
// -------------------------------------------------------------
static void draw_appstore_content(window_t* win) {
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT + 1;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 3;

    gfx_draw_rect(cx, cy, cw, ch, 0x00141418);

    gfx_draw_string_transparent(cx + 16, cy + 12, "Featured Applications for RatanaOS", COLOR_WHITE);

    // 2 App Store Cards
    int card_w = (cw - 48) / 2;
    // Card 1: Xcode
    int c1_x = cx + 16;
    int cy_pos = cy + 36;
    gfx_draw_rounded_rect(c1_x, cy_pos, card_w, 160, 8, 0x00202028);
    gfx_draw_rounded_rect_outline(c1_x, cy_pos, card_w, 160, 8, 0x003A3A44);
    icon_draw_terminal_48(c1_x + 14, cy_pos + 14);
    gfx_draw_string_transparent(c1_x + 72, cy_pos + 16, "Xcode Pro", COLOR_WHITE);
    gfx_draw_string_transparent(c1_x + 72, cy_pos + 36, "Developer Tools", 0x008E8E93);
    gfx_draw_string_transparent(c1_x + 14, cy_pos + 76, "Native 64-bit IDE & C Compiler", COLOR_WHITE);
    gfx_draw_rounded_rect(c1_x + 14, cy_pos + 116, 72, 24, 12, 0x000A84FF);
    gfx_draw_string_transparent(c1_x + 26, cy_pos + 120, "GET", COLOR_WHITE);

    // Card 2: Logic Pro
    int c2_x = c1_x + card_w + 16;
    gfx_draw_rounded_rect(c2_x, cy_pos, card_w, 160, 8, 0x00202028);
    gfx_draw_rounded_rect_outline(c2_x, cy_pos, card_w, 160, 8, 0x003A3A44);
    icon_draw_music_48(c2_x + 14, cy_pos + 14);
    gfx_draw_string_transparent(c2_x + 72, cy_pos + 16, "Logic Pro", COLOR_WHITE);
    gfx_draw_string_transparent(c2_x + 72, cy_pos + 36, "Music Production", 0x008E8E93);
    gfx_draw_string_transparent(c2_x + 14, cy_pos + 76, "Multi-track Audio Studio", COLOR_WHITE);
    gfx_draw_rounded_rect(c2_x + 14, cy_pos + 116, 92, 24, 12, 0x0030D158);
    gfx_draw_string_transparent(c2_x + 20, cy_pos + 120, "INSTALLED", COLOR_WHITE);
}

// -------------------------------------------------------------
// App 11: About This Mac Dialog Window (480x320)
// -------------------------------------------------------------
static void draw_about_content(window_t* win) {
    int cx = win->x + 24;
    int cy = win->y + TITLEBAR_HEIGHT + 24;

    // Large 64x64  Apple Emblem
    icon_draw_apple_64(cx, cy + 10);

    // Specification details (read from live hardware & kernel state)
    int tx = cx + 84;
    gfx_draw_string_transparent(tx, cy + 4, "RatanaOS Sequoia (2026 64-bit Edition)", COLOR_WHITE);
    gfx_draw_string_transparent(tx, cy + 26, "Version 15.4 (Build 2026.09)", 0x008E8E93);

    gfx_draw_line(tx, cy + 50, tx + 330, cy + 50, 0x003A3A3C);

    gfx_draw_string_transparent(tx, cy + 62, "Model:     MacBook Pro (x86_64 Long Mode)", COLOR_WHITE);

    cpu_info_t cpu = cpuid_get_info();
    char chip_str[64];
    strcpy(chip_str, "Chip:      ");
    strncat(chip_str, cpu.brand[0] ? cpu.brand : "64-bit Long Mode Processor", 32);
    gfx_draw_string_transparent(tx, cy + 86, chip_str, COLOR_WHITE);

    heap_stats_t hstats = heap_get_stats();
    char mem_str[64];
    strcpy(mem_str, "Memory:    256 MB RAM (");
    char h_kb[16];
    utoa((uint32_t)(hstats.total_size / (1024 * 1024)), h_kb, 10);
    strcat(mem_str, h_kb);
    strcat(mem_str, " MB Dynamic Heap)");
    gfx_draw_string_transparent(tx, cy + 110, mem_str, COLOR_WHITE);

    gfx_draw_string_transparent(tx, cy + 134, "Startup:   RatanaOS HD (1024 MB Boot Volume)", COLOR_WHITE);

    char gfx_str[64];
    strcpy(gfx_str, "Graphics:  VBE 32-bit Linear Framebuffer (");
    char w_s[8], h_s[8];
    utoa((uint32_t)gfx_get_width(), w_s, 10);
    utoa((uint32_t)gfx_get_height(), h_s, 10);
    strcat(gfx_str, w_s);
    strcat(gfx_str, "x");
    strcat(gfx_str, h_s);
    strcat(gfx_str, ")");
    gfx_draw_string_transparent(tx, cy + 158, gfx_str, COLOR_WHITE);

    gfx_draw_string_transparent(tx, cy + 182, "Serial:    C02RTN2026X86", 0x008E8E93);

    // System Report Button
    gfx_draw_rounded_rect(tx, cy + 210, 140, 26, 6, 0x003A3A3C);
    gfx_draw_rounded_rect_outline(tx, cy + 210, 140, 26, 6, 0x0050505A);
    gfx_draw_string_transparent(tx + 12, cy + 216, "System Report...", COLOR_WHITE);
}

// -------------------------------------------------------------
// §5. macOS Desktop Compositor & Event Handling
// -------------------------------------------------------------
static void draw_macos_desktop(void) {
    int sw = gfx_get_width();

    // 1. Wallpaper Precomputed Gradient
    gfx_draw_wallpaper();

    // 2. Desktop Icons on Right Side
    // 2a. Drive Icon (RatanaOS HD)
    icon_draw_drive_48(sw - 74, 40);
    gfx_draw_string_transparent(sw - 88, 92, "RatanaOS HD", COLOR_WHITE);

    // 2b. Applications Folder
    icon_draw_folder_48(sw - 74, 130);
    gfx_draw_string_transparent(sw - 94, 182, "Applications", COLOR_WHITE);

    // 2c. Documents Folder
    icon_draw_folder_48(sw - 74, 220);
    gfx_draw_string_transparent(sw - 86, 272, "Documents", COLOR_WHITE);

    // 3. Render Windows in Z-Order
    for (int i = 0; i < window_count; i++) {
        draw_window(&windows[i]);
    }

    // 4. Render macOS Top Menu Bar
    const char* active_name = "Finder";
    for (int i = window_count - 1; i >= 0; i--) {
        if (windows[i].is_open && !windows[i].is_minimized && windows[i].is_active) {
            active_name = windows[i].title;
            break;
        }
    }
    menubar_draw(active_name);

    // 5. Render macOS Floating Bottom Dock
    bool open_states[12];
    bool active_states[12];
    for (int i = 0; i < 12 && i < window_count; i++) {
        open_states[i] = windows[i].is_open;
        active_states[i] = windows[i].is_active;
    }
    dock_draw(open_states, active_states, window_count);
}

static void process_gui_events(void) {
    int mx = mouse_get_x();
    int my = mouse_get_y();
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
            windows[11].is_open = true;
            windows[11].is_minimized = false;
            focus_window(11);
            prev_left_click = left_click;
            return;
        } else if (m_hit == MENUBAR_HIT_ITEM_SETTINGS) {
            windows[9].is_open = true;
            windows[9].is_minimized = false;
            focus_window(9);
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
                if (!windows[dock_idx].is_open) {
                    windows[dock_idx].is_open = true;
                    windows[dock_idx].is_minimized = false;
                    focus_window(dock_idx);
                } else if (windows[dock_idx].is_minimized) {
                    windows[dock_idx].is_minimized = false;
                    focus_window(dock_idx);
                } else if (windows[dock_idx].is_active) {
                    windows[dock_idx].is_minimized = true;
                } else {
                    focus_window(dock_idx);
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
            windows[0].is_open = true; // Finder
            windows[0].is_minimized = false;
            focus_window(0);
            prev_left_click = left_click;
            return;
        }
        // 3b. Applications Folder
        if (mx >= sw - 94 && mx <= sw - 20 && my >= 130 && my <= 200) {
            windows[1].is_open = true; // Launchpad
            windows[1].is_minimized = false;
            focus_window(1);
            prev_left_click = left_click;
            return;
        }
        // 3c. Documents Folder
        if (mx >= sw - 88 && mx <= sw - 20 && my >= 220 && my <= 290) {
            windows[7].is_open = true; // Notes
            windows[7].is_minimized = false;
            focus_window(7);
            prev_left_click = left_click;
            return;
        }

        // 4. Window Hit Testing & Traffic Lights
        for (int i = window_count - 1; i >= 0; i--) {
            window_t* win = &windows[i];
            if (!win->is_open || win->is_minimized) continue;

            if (mx >= win->x && mx < win->x + win->width && my >= win->y && my < win->y + win->height) {
                focus_window(i);

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

    // Paint Continuous Drag Drawing
    if (left_click) {
        for (int i = window_count - 1; i >= 0; i--) {
            window_t* win = &windows[i];
            if (win->is_open && !win->is_minimized && win->id == 6) { // Paint Studio (id=6)
                if (mx >= win->x + 8 && mx < win->x + 8 + CANVAS_W &&
                    my >= win->y + TITLEBAR_HEIGHT + 38 && my < win->y + TITLEBAR_HEIGHT + 38 + CANVAS_H) {
                    if (win->handle_click) {
                        win->handle_click(win, mx - win->x, my - win->y, 1);
                    }
                }
                break;
            }
        }
    }

    prev_left_click = left_click;
}

// -------------------------------------------------------------
// GUI Lifecycle
// -------------------------------------------------------------
void gui_init(multiboot_info_t* mbi) {
    gfx_init(mbi);
    mouse_init();
    mouse_set_bounds(gfx_get_width(), gfx_get_height());

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
    create_window(9, "System Settings", 250, 100, 500, 320, draw_settings_content, NULL);
    create_window(10, "App Store", 260, 110, 500, 300, draw_appstore_content, NULL);
    create_window(11, "About This Mac", 272, 160, 480, 320, draw_about_content, NULL);

    // Initial desktop state: Open Finder and About This Mac
    windows[0].is_open = true;
    windows[11].is_open = true;
    focus_window(11);
}

void gui_start(void) {
    gui_running = true;

    while (gui_running) {
        if (keyboard_has_key()) {
            char k = keyboard_getchar();
            if (k == 27) { // ESC
                gui_exit();
                break;
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

