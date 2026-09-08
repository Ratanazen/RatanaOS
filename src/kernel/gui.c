#include "../include/gui.h"
#include "../include/gfx.h"
#include "../include/icons.h"
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

#define MENUBAR_HEIGHT 24
#define TITLEBAR_HEIGHT 28
#define DOCK_HEIGHT 54
#define DOCK_ICON_SIZE 32

static window_t windows[MAX_WINDOWS];
static int window_count = 0;
static bool gui_running = false;
static bool apple_menu_open = false;
static bool prev_left_click = false;

// Calculator state
static char calc_display[32] = "0";
static int  calc_op1 = 0;
static char calc_op = 0;
static bool calc_clear_on_next = false;

// Paint Canvas state
#define CANVAS_W 340
#define CANVAS_H 180
static uint32_t paint_canvas[CANVAS_W * CANVAS_H];
static uint32_t paint_cur_color = 0x000A84FF; // macOS Blue

// Forward declarations of app content renderers
static void draw_finder_content(window_t* win);
static void draw_terminal_content(window_t* win);
static void draw_sysmon_content(window_t* win);
static void draw_calc_content(window_t* win);
static void draw_paint_content(window_t* win);
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
    win->is_open = true;
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
// App 0: Finder (Files & Folders)
// -------------------------------------------------------------
static void draw_finder_content(window_t* win) {
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 2;

    // Split View: Left Sidebar (120px) + Right Content View
    int sidebar_w = 120;
    gfx_draw_rect(cx, cy, sidebar_w, ch, 0x00242426);
    gfx_draw_line(cx + sidebar_w, cy, cx + sidebar_w, cy + ch, 0x003A3A3C);

    gfx_draw_string_transparent(cx + 10, cy + 10, "Favorites", 0x008E8E93);
    gfx_draw_string_transparent(cx + 14, cy + 30, "> Applications", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 50, "> Desktop", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 70, "> Documents", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 90, "> Downloads", COLOR_TEXT_WHITE);

    gfx_draw_string_transparent(cx + 10, cy + 120, "Locations", 0x008E8E93);
    gfx_draw_string_transparent(cx + 14, cy + 140, "[=] RatanaOS HD", 0x000A84FF);

    // Right file grid
    int rx = cx + sidebar_w + 12;
    gfx_draw_rect(rx, cy, cw - sidebar_w - 12, ch, 0x001C1C1E);

    // Folder Items
    icon_draw_drive(rx + 16, cy + 16);
    gfx_draw_string_transparent(rx + 4, cy + 46, "RatanaOS HD", COLOR_TEXT_WHITE);

    icon_draw_finder(rx + 106, cy + 14, 28);
    gfx_draw_string_transparent(rx + 100, cy + 46, "System", COLOR_TEXT_WHITE);

    icon_draw_terminal(rx + 186, cy + 14, 28);
    gfx_draw_string_transparent(rx + 172, cy + 46, "Applications", COLOR_TEXT_WHITE);

    icon_draw_about(rx + 266, cy + 14, 28);
    gfx_draw_string_transparent(rx + 262, cy + 46, "Library", COLOR_TEXT_WHITE);

    // Bottom status bar
    gfx_draw_line(cx + sidebar_w, cy + ch - 22, cx + cw, cy + ch - 22, 0x003A3A3C);
    gfx_draw_string_transparent(rx + 8, cy + ch - 16, "4 items, 256 MB available on RatanaOS HD", 0x008E8E93);
}

// -------------------------------------------------------------
// App 1: Terminal.app
// -------------------------------------------------------------
static void draw_terminal_content(window_t* win) {
    int cx = win->x + 2;
    int cy = win->y + TITLEBAR_HEIGHT;
    int cw = win->width - 4;
    int ch = win->height - TITLEBAR_HEIGHT - 2;

    gfx_draw_rect(cx, cy, cw, ch, 0x00101012);

    gfx_draw_string_transparent(cx + 10, cy + 8, "Last login: Tue Sep 8 22:33:00 on ttys000", 0x008E8E93);
    gfx_draw_string_transparent(cx + 10, cy + 28, "RatanaOS 64-bit (x86_64 Long Mode) Darwin 24.0.0", 0x0030D158);
    gfx_draw_string_transparent(cx + 10, cy + 52, "reny@macbook-pro ~ %", 0x000A84FF);
    gfx_draw_string_transparent(cx + 186, cy + 52, "neofetch", COLOR_WHITE);

    // Neofetch Apple Logo & Info
    icon_draw_apple(cx + 16, cy + 80, 0x0030D158);
    gfx_draw_string_transparent(cx + 42, cy + 76, "reny@RatanaOS-MacBookPro", 0x000A84FF);
    gfx_draw_string_transparent(cx + 42, cy + 94, "------------------------", 0x003A3A3C);
    gfx_draw_string_transparent(cx + 42, cy + 112, "OS:      RatanaOS Sequoia 15.4 (64-bit)", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(cx + 42, cy + 130, "Host:    MacBookPro18,1 (x86_64)", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(cx + 42, cy + 148, "Kernel:  64-bit Long Mode Kernel", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(cx + 42, cy + 166, "Shell:   zsh 5.9 (RatanaSH)", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(cx + 42, cy + 184, "DE:      Aqua / RatanaWM 64-bit", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(cx + 42, cy + 202, "Memory:  256MB / 16MB Heap", COLOR_TEXT_WHITE);

    // Terminal prompt cursor
    gfx_draw_string_transparent(cx + 10, cy + 226, "reny@macbook-pro ~ %", 0x000A84FF);
    uint32_t ticks = timer_get_ticks();
    if ((ticks / 50) % 2 == 0) {
        gfx_draw_rect(cx + 186, cy + 226, 8, 16, COLOR_WHITE);
    }
}

// -------------------------------------------------------------
// App 2: Activity Monitor.app
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

    // CPU Info
    char cpu_str[64];
    strcpy(cpu_str, "Chip: ");
    strcat(cpu_str, cpu.brand[0] ? cpu.brand : "x86_64 Processor");
    gfx_draw_string_transparent(cx + 8, cy + 28, cpu_str, COLOR_TEXT_WHITE);

    // Uptime
    char upt_str[32];
    strcpy(upt_str, "System Uptime: ");
    char num_buf[16];
    utoa(uptime_sec, num_buf, 10);
    strcat(upt_str, num_buf);
    strcat(upt_str, " s");
    gfx_draw_string_transparent(cx + 8, cy + 46, upt_str, 0x008E8E93);

    // RAM Bar
    gfx_draw_string_transparent(cx + 8, cy + 72, "Physical Memory (256 MB Unified)", COLOR_TEXT_WHITE);
    int bar_w = win->width - 32;
    int used_pgs = (int)(total_pgs - free_pgs);
    int pmm_fill = total_pgs > 0 ? (used_pgs * bar_w) / (int)total_pgs : 0;
    gfx_draw_rect(cx + 8, cy + 90, bar_w, 16, 0x002C2C2E);
    gfx_draw_rect(cx + 8, cy + 90, pmm_fill < 12 ? 12 : pmm_fill, 16, 0x0030D158); // macOS Green
    gfx_draw_rect_outline(cx + 8, cy + 90, bar_w, 16, 0x003A3A3C);

    // Kernel Heap Bar
    gfx_draw_string_transparent(cx + 8, cy + 118, "Memory Used (16384 KB Dynamic Heap)", COLOR_TEXT_WHITE);
    int heap_fill = hstats.total_size > 0 ? (int)((hstats.used_size * bar_w) / hstats.total_size) : 0;
    gfx_draw_rect(cx + 8, cy + 136, bar_w, 16, 0x002C2C2E);
    gfx_draw_rect(cx + 8, cy + 136, heap_fill < 12 ? 12 : heap_fill, 16, 0x00FF9F0A); // macOS Orange
    gfx_draw_rect_outline(cx + 8, cy + 136, bar_w, 16, 0x003A3A3C);

    // Process List Table
    gfx_draw_rect(cx + 8, cy + 162, bar_w, 80, 0x001C1C1E);
    gfx_draw_rect_outline(cx + 8, cy + 162, bar_w, 80, 0x003A3A3C);
    gfx_draw_string_transparent(cx + 14, cy + 168, "Process Name       % CPU    Memory    Threads", 0x008E8E93);
    gfx_draw_line(cx + 8, cy + 184, cx + 8 + bar_w, cy + 184, 0x003A3A3C);
    gfx_draw_string_transparent(cx + 14, cy + 190, "kernel_task         1.2%     24 MB       8", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 206, "WindowServer        3.4%     16 MB       4", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(cx + 14, cy + 222, "Finder              0.2%      8 MB       2", COLOR_TEXT_WHITE);
}

// -------------------------------------------------------------
// App 3: Calculator.app (macOS Style)
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

    // Display Screen
    gfx_draw_rect(cx, cy, 216, 42, 0x001C1C1E);
    gfx_draw_string_transparent(cx + 190 - (int)strlen(calc_display) * 8, cy + 14, calc_display, COLOR_WHITE);

    // Button Matrix
    int btn_w = 48;
    int btn_h = 34;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int bx = cx + c * (btn_w + 8);
            int by = cy + 50 + r * (btn_h + 8);
            uint32_t bg = (c == 3) ? 0x00FF9F0A : (r == 0 ? 0x00505052 : 0x003A3A3C);
            gfx_draw_rect(bx, by, btn_w, btn_h, bg);
            gfx_draw_rect_outline(bx, by, btn_w, btn_h, 0x002C2C2E);
            gfx_draw_string_transparent(bx + 18, by + 10, calc_buttons[r][c], COLOR_WHITE);
        }
    }

    // Zero & Equals Bottom Row
    int by = cy + 50 + 4 * (btn_h + 8);
    gfx_draw_rect(cx, by, 104, btn_h, 0x003A3A3C);
    gfx_draw_string_transparent(cx + 46, by + 10, "0", COLOR_WHITE);

    gfx_draw_rect(cx + 112, by, btn_w, btn_h, 0x003A3A3C);
    gfx_draw_string_transparent(cx + 130, by + 10, ".", COLOR_WHITE);

    gfx_draw_rect(cx + 168, by, btn_w, btn_h, 0x00FF9F0A);
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

    // Check Bottom Row (= and 0)
    int by = cy + 4 * (btn_h + 8);
    if (rel_y >= by && rel_y < by + btn_h) {
        if (rel_x >= cx && rel_x < cx + 104) { // 0
            if (calc_clear_on_next || strcmp(calc_display, "0") == 0) {
                strcpy(calc_display, "0");
                calc_clear_on_next = false;
            } else if (strlen(calc_display) < 10) {
                strcat(calc_display, "0");
            }
        } else if (rel_x >= cx + 168 && rel_x < cx + 168 + btn_w) { // =
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
// App 4: Paint Studio.app
// -------------------------------------------------------------
static const uint32_t paint_colors[8] = {
    0x00FFFFFF, 0x00FF453A, 0x0030D158, 0x000A84FF,
    0x00FFD60A, 0x005AC8FA, 0x00BF5AF2, 0x001C1C1E
};

static void draw_paint_content(window_t* win) {
    int cx = win->x + 8;
    int cy = win->y + TITLEBAR_HEIGHT + 8;

    // Palette Bar
    for (int i = 0; i < 8; i++) {
        int px = cx + i * 28;
        int py = cy;
        gfx_draw_rect(px, py, 22, 22, paint_colors[i]);
        gfx_draw_rect_outline(px, py, 22, 22, (paint_cur_color == paint_colors[i]) ? COLOR_WHITE : 0x003A3A3C);
    }

    // Canvas Frame
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
// App 5: About This Mac (macOS System Dialog)
// -------------------------------------------------------------
static void draw_about_content(window_t* win) {
    int cx = win->x + 20;
    int cy = win->y + TITLEBAR_HEIGHT + 16;

    // Big  Apple Emblem on Left
    icon_draw_about(cx + 8, cy + 10, 48);

    // System Information Text
    int tx = cx + 72;
    gfx_draw_string_transparent(tx, cy + 4, "RatanaOS Sequoia", COLOR_WHITE);
    gfx_draw_string_transparent(tx, cy + 24, "Version 15.4 (2026 Edition)", 0x008E8E93);

    gfx_draw_line(tx, cy + 46, tx + 240, cy + 46, 0x003A3A3C);

    gfx_draw_string_transparent(tx, cy + 56, "Model:     MacBook Pro (x86_64)", COLOR_TEXT_WHITE);

    cpu_info_t cpu = cpuid_get_info();
    char chip_str[48];
    strcpy(chip_str, "Chip:      ");
    strncat(chip_str, cpu.brand[0] ? cpu.brand : "64-bit Long Mode CPU", 26);
    gfx_draw_string_transparent(tx, cy + 76, chip_str, COLOR_TEXT_WHITE);

    gfx_draw_string_transparent(tx, cy + 96, "Memory:    256 MB Unified Memory", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(tx, cy + 116, "Startup:   RatanaOS HD (1GB)", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(tx, cy + 136, "Graphics:  VBE 32-bit Framebuffer", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(tx, cy + 156, "Serial:    C02RTN2026X86", 0x008E8E93);

    // System Report button
    gfx_draw_rect(tx, cy + 180, 130, 24, 0x003A3A3C);
    gfx_draw_rect_outline(tx, cy + 180, 130, 24, 0x0048484A);
    gfx_draw_string_transparent(tx + 12, cy + 184, "System Report...", COLOR_WHITE);
}

// -------------------------------------------------------------
// macOS Window Decorator (Traffic Light Buttons)
// -------------------------------------------------------------
static void draw_window(window_t* win) {
    if (!win->is_open || win->is_minimized) return;

    // Window Drop Shadow
    gfx_draw_rect(win->x + 6, win->y + 6, win->width, win->height, 0x00050508);

    // Window Main Body
    gfx_draw_rect(win->x, win->y, win->width, win->height, 0x001E1E24);
    gfx_draw_rect_outline(win->x, win->y, win->width, win->height, win->is_active ? 0x0048484A : 0x002C2C2E);

    // macOS Titlebar
    gfx_draw_rect(win->x + 1, win->y + 1, win->width - 2, TITLEBAR_HEIGHT - 1, 0x0028282E);
    gfx_draw_line(win->x, win->y + TITLEBAR_HEIGHT, win->x + win->width, win->y + TITLEBAR_HEIGHT, 0x003A3A3C);

    // 🔴 🟡 🟢 macOS Traffic Light Window Controls
    // Red (Close)
    gfx_draw_circle(win->x + 14, win->y + 14, 5, 0x00FF5F56);
    // Yellow (Minimize)
    gfx_draw_circle(win->x + 30, win->y + 14, 5, 0x00FFBD2E);
    // Green (Zoom / Maximize)
    gfx_draw_circle(win->x + 46, win->y + 14, 5, 0x0027C93F);

    // Centered Window Title
    int title_len = (int)strlen(win->title) * 8;
    int title_x = win->x + (win->width - title_len) / 2;
    gfx_draw_string_transparent(title_x, win->y + 6, win->title, win->is_active ? COLOR_WHITE : 0x008E8E93);

    // Draw application content
    if (win->draw_content) {
        win->draw_content(win);
    }
}

// -------------------------------------------------------------
// macOS Top Menu Bar & Bottom Floating Dock
// -------------------------------------------------------------
static void draw_macos_desktop_environment(void) {
    int sw = gfx_get_width();
    int sh = gfx_get_height();

    // 1. macOS Sequoia Gradient Wallpaper (Deep Navy to Violet-Black)
    gfx_draw_gradient_v(0, 0, sw, sh, 0x00121B2A, 0x00080B12);

    // 2. Desktop Drive Icon (RatanaOS HD) on Top-Right Desktop
    icon_draw_drive(sw - 64, 40);
    gfx_draw_string_transparent(sw - 84, 70, "RatanaOS HD", COLOR_WHITE);

    // 3. Render Windows (Z-order)
    for (int i = 0; i < window_count; i++) {
        draw_window(&windows[i]);
    }

    // 4. macOS Top Menu Bar
    gfx_draw_rect(0, 0, sw, MENUBAR_HEIGHT, 0x001C1C1E);
    gfx_draw_line(0, MENUBAR_HEIGHT, sw, MENUBAR_HEIGHT, 0x003A3A3C);

    //  Apple Logo on top-left
    icon_draw_apple(12, 4, apple_menu_open ? 0x000A84FF : COLOR_WHITE);

    // Active Application Name (Bold)
    const char* active_app = "Finder";
    for (int i = window_count - 1; i >= 0; i--) {
        if (windows[i].is_open && !windows[i].is_minimized && windows[i].is_active) {
            active_app = windows[i].title;
            break;
        }
    }
    gfx_draw_string_transparent(36, 4, active_app, COLOR_WHITE);

    // Menu Bar Items
    int mx = 36 + (int)strlen(active_app) * 8 + 16;
    gfx_draw_string_transparent(mx, 4, "File", 0x00D1D1D6);
    gfx_draw_string_transparent(mx + 44, 4, "Edit", 0x00D1D1D6);
    gfx_draw_string_transparent(mx + 88, 4, "View", 0x00D1D1D6);
    gfx_draw_string_transparent(mx + 132, 4, "Window", 0x00D1D1D6);
    gfx_draw_string_transparent(mx + 192, 4, "Help", 0x00D1D1D6);

    // Menu Bar Status Extras (Right side)
    icon_draw_wifi(sw - 235, 8, COLOR_WHITE);
    icon_draw_battery(sw - 215, 6, COLOR_WHITE);

    // RAM Widget
    heap_stats_t hstats = heap_get_stats();
    char ram_str[24];
    strcpy(ram_str, "");
    char rn[12];
    utoa((uint32_t)(hstats.used_size / 1024), rn, 10);
    strcat(ram_str, rn);
    strcat(ram_str, " KB");
    gfx_draw_string_transparent(sw - 190, 4, ram_str, 0x0030D158);

    // Real-Time Clock
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

    gfx_draw_string_transparent(sw - 128, 4, clock_str, COLOR_WHITE);
    icon_draw_search(sw - 38, 5, COLOR_WHITE);
    icon_draw_control_center(sw - 20, 6, COLOR_WHITE);

    //  Apple Menu Dropdown
    if (apple_menu_open) {
        int am_w = 200;
        int am_h = 175;
        gfx_draw_rect(8, MENUBAR_HEIGHT + 2, am_w, am_h, 0x00242428);
        gfx_draw_rect_outline(8, MENUBAR_HEIGHT + 2, am_w, am_h, 0x003A3A3C);

        const char* am_items[] = {
            "About This Mac",
            "System Settings...",
            "App Store...",
            "Recent Items >",
            "Force Quit...",
            "Restart...",
            "Shut Down...",
            "Exit to CLI"
        };

        for (int i = 0; i < 8; i++) {
            uint32_t col = (i == 7) ? 0x00FF453A : COLOR_TEXT_WHITE;
            gfx_draw_string_transparent(18, MENUBAR_HEIGHT + 10 + i * 20, am_items[i], col);
        }
    }

    // 5. macOS Floating Bottom Dock
    int dock_w = 340;
    int dock_x = (sw - dock_w) / 2;
    int dock_y = sh - DOCK_HEIGHT - 12;

    // Frosted Glass Dock Pill
    gfx_draw_rect(dock_x, dock_y, dock_w, DOCK_HEIGHT, 0x00282830);
    gfx_draw_rect_outline(dock_x, dock_y, dock_w, DOCK_HEIGHT, 0x00484852);

    // 6 App Icons + Separator + Trash Icon
    int icon_start_x = dock_x + 14;
    int icon_y = dock_y + 8;

    // Icon 0: Finder
    icon_draw_finder(icon_start_x, icon_y, DOCK_ICON_SIZE);
    // Icon 1: Terminal
    icon_draw_terminal(icon_start_x + 44, icon_y, DOCK_ICON_SIZE);
    // Icon 2: Activity Monitor
    icon_draw_sysmon(icon_start_x + 88, icon_y, DOCK_ICON_SIZE);
    // Icon 3: Calculator
    icon_draw_calculator(icon_start_x + 132, icon_y, DOCK_ICON_SIZE);
    // Icon 4: Paint Studio
    icon_draw_paint(icon_start_x + 176, icon_y, DOCK_ICON_SIZE);
    // Icon 5: About This Mac
    icon_draw_about(icon_start_x + 220, icon_y, DOCK_ICON_SIZE);

    // Dock Divider Line
    gfx_draw_line(icon_start_x + 264, dock_y + 8, icon_start_x + 264, dock_y + DOCK_HEIGHT - 8, 0x00484852);

    // Icon 6: Trash
    icon_draw_trash(icon_start_x + 276, icon_y, DOCK_ICON_SIZE);

    // Active App Dot Indicators (Below open windows)
    for (int i = 0; i < 6 && i < window_count; i++) {
        if (windows[i].is_open) {
            int dot_x = icon_start_x + i * 44 + 14;
            int dot_y = dock_y + DOCK_HEIGHT - 6;
            gfx_draw_rect(dot_x, dot_y, 4, 3, windows[i].is_active ? 0x000A84FF : COLOR_WHITE);
        }
    }
}

// -------------------------------------------------------------
// Mouse & Event Processing
// -------------------------------------------------------------
static void process_gui_events(void) {
    int mx = mouse_get_x();
    int my = mouse_get_y();
    bool left_click = mouse_is_left_clicked();
    bool click_down = left_click && !prev_left_click;
    int sw = gfx_get_width();
    int sh = gfx_get_height();

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
        // 1. Check  Apple Menu Dropdown Click
        if (apple_menu_open) {
            if (mx >= 8 && mx <= 208 && my >= MENUBAR_HEIGHT && my <= MENUBAR_HEIGHT + 175) {
                int item = (my - (MENUBAR_HEIGHT + 10)) / 20;
                if (item == 0) { // About This Mac
                    windows[5].is_open = true;
                    windows[5].is_minimized = false;
                    focus_window(5);
                } else if (item == 1) { // System Settings
                    windows[2].is_open = true;
                    windows[2].is_minimized = false;
                    focus_window(2);
                } else if (item == 7) { // Exit to CLI
                    gui_exit();
                    return;
                }
                apple_menu_open = false;
                prev_left_click = left_click;
                return;
            } else {
                apple_menu_open = false;
            }
        }

        // 2. Check  Apple Logo Click
        if (mx >= 8 && mx <= 30 && my >= 0 && my <= MENUBAR_HEIGHT) {
            apple_menu_open = !apple_menu_open;
            prev_left_click = left_click;
            return;
        }

        // 3. Check Floating Dock Clicks
        int dock_w = 340;
        int dock_x = (sw - dock_w) / 2;
        int dock_y = sh - DOCK_HEIGHT - 12;

        if (mx >= dock_x && mx <= dock_x + dock_w && my >= dock_y && my <= dock_y + DOCK_HEIGHT) {
            int icon_start_x = dock_x + 14;
            int rel = mx - icon_start_x;
            if (rel >= 0) {
                int icon_idx = rel / 44;
                if (icon_idx >= 0 && icon_idx < 6 && icon_idx < window_count) {
                    if (windows[icon_idx].is_minimized) {
                        windows[icon_idx].is_minimized = false;
                        focus_window(icon_idx);
                    } else if (windows[icon_idx].is_active) {
                        windows[icon_idx].is_minimized = true;
                    } else {
                        windows[icon_idx].is_open = true;
                        focus_window(icon_idx);
                    }
                } else if (icon_idx == 6) { // Trash can
                    memset(paint_canvas, 0, sizeof(paint_canvas)); // Clear paint canvas
                }
            }
            prev_left_click = left_click;
            return;
        }

        // 4. Check Desktop Drive Icon Double-Click
        if (mx >= sw - 84 && mx <= sw - 20 && my >= 40 && my <= 90) {
            windows[0].is_open = true; // Open Finder
            windows[0].is_minimized = false;
            focus_window(0);
            prev_left_click = left_click;
            return;
        }

        // 5. Check Window Titlebars & Traffic Light Controls
        for (int i = window_count - 1; i >= 0; i--) {
            window_t* win = &windows[i];
            if (!win->is_open || win->is_minimized) continue;

            if (mx >= win->x && mx < win->x + win->width && my >= win->y && my < win->y + win->height) {
                focus_window(i);

                if (my < win->y + TITLEBAR_HEIGHT) {
                    // Red traffic light (Close)
                    if (mx >= win->x + 8 && mx <= win->x + 20) {
                        win->is_open = false;
                    }
                    // Yellow traffic light (Minimize)
                    else if (mx >= win->x + 24 && mx <= win->x + 36) {
                        win->is_minimized = true;
                    }
                    // Green traffic light (Zoom)
                    else if (mx >= win->x + 40 && mx <= win->x + 52) {
                        win->width = 480;
                        win->height = 320;
                    }
                    // Titlebar drag
                    else {
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

    // Paint continuous drawing drag
    if (left_click) {
        for (int i = window_count - 1; i >= 0; i--) {
            window_t* win = &windows[i];
            if (win->is_open && !win->is_minimized && win->id == 4) { // Paint Studio app
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
// GUI Life Cycle & Initialization
// -------------------------------------------------------------
void gui_init(multiboot_info_t* mbi) {
    gfx_init(mbi);
    mouse_init();
    mouse_set_bounds(gfx_get_width(), gfx_get_height());

    window_count = 0;
    memset(paint_canvas, 0, sizeof(paint_canvas));

    // Register 6 macOS Desktop Applications
    create_window(0, "Finder", 40, 48, 480, 260, draw_finder_content, NULL);
    create_window(1, "Terminal", 540, 48, 460, 260, draw_terminal_content, NULL);
    create_window(2, "Activity Monitor", 540, 320, 460, 280, draw_sysmon_content, NULL);
    create_window(3, "Calculator", 40, 320, 240, 280, draw_calc_content, handle_calc_click);
    create_window(4, "Paint Studio", 300, 320, 360, 280, draw_paint_content, handle_paint_click);
    create_window(5, "About This Mac", 320, 140, 380, 240, draw_about_content, NULL);

    // Initial focus on About This Mac dialog & Finder
    focus_window(5);
}

void gui_start(void) {
    gui_running = true;

    while (gui_running) {
        if (keyboard_has_key()) {
            char k = keyboard_getchar();
            if (k == 27) { // ESC exits GUI
                gui_exit();
                break;
            }
        }

        process_gui_events();
        draw_macos_desktop_environment();
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
