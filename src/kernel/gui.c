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

// Forward declarations
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
    win->orig_w = w;
    win->orig_h = h;
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
// §3. macOS Window Chrome & Traffic Lights (🔴 🟡 🟢)
// -------------------------------------------------------------
static titlebar_hit_t window_hit_test_titlebar(window_t* win, int mx, int my) {
    if (mx < win->x || mx >= win->x + win->width || my < win->y || my >= win->y + TITLEBAR_HEIGHT) {
        return TITLEBAR_HIT_NONE;
    }

    int btn_cy = win->y + 14;

    // 🔴 Red Close Button (x + 14, radius 6)
    int dx = mx - (win->x + 14), dy = my - btn_cy;
    if (dx * dx + dy * dy <= 36) return TITLEBAR_HIT_CLOSE;

    // 🟡 Yellow Minimize Button (x + 32, radius 6)
    dx = mx - (win->x + 32);
    if (dx * dx + dy * dy <= 36) return TITLEBAR_HIT_MINIMIZE;

    // 🟢 Green Zoom Button (x + 50, radius 6)
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

    // 🔴 🟡 🟢 Traffic Lights (12px Diameter, 8px Margin, 8px Spacing)
    // Red Close
    gfx_draw_circle(win->x + 14, win->y + 14, 6, COLOR_MAC_CLOSE);
    // Yellow Minimize
    gfx_draw_circle(win->x + 32, win->y + 14, 6, COLOR_MAC_MIN);
    // Green Zoom / Maximize
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

    int rx = cx + sidebar_w + 16;
    gfx_draw_rect(cx + sidebar_w + 1, cy, cw - sidebar_w - 1, ch, 0x001C1C1E);

    icon_draw_drive_48(rx, cy + 16);
    gfx_draw_string_transparent(rx - 8, cy + 56, "RatanaOS HD", COLOR_WHITE);

    icon_draw_finder_48(rx + 90, cy + 16);
    gfx_draw_string_transparent(rx + 94, cy + 56, "System", COLOR_WHITE);

    icon_draw_terminal_48(rx + 180, cy + 16);
    gfx_draw_string_transparent(rx + 168, cy + 56, "Applications", COLOR_WHITE);

    icon_draw_about_48(rx + 270, cy + 16);
    gfx_draw_string_transparent(rx + 272, cy + 56, "Library", COLOR_WHITE);

    gfx_draw_line(cx + sidebar_w, cy + ch - 24, cx + cw, cy + ch - 24, 0x003A3A3C);
    gfx_draw_string_transparent(rx, cy + ch - 18, "4 items, 256 MB available on RatanaOS HD", 0x008E8E93);
}

// -------------------------------------------------------------
// App 1: Terminal.app Window
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
// App 2: Activity Monitor.app Window
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
// App 3: Calculator.app Window
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
// App 4: Paint Studio.app Window
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
// §4. About This Mac Dialog Window (480x320)
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

    // 2. Desktop Drive Icon (RatanaOS HD) at Top-Right
    icon_draw_drive_48(sw - 74, 40);
    gfx_draw_string_transparent(sw - 88, 92, "RatanaOS HD", COLOR_WHITE);

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
    bool open_states[6];
    bool active_states[6];
    for (int i = 0; i < 6 && i < window_count; i++) {
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
            windows[5].is_open = true;
            windows[5].is_minimized = false;
            focus_window(5);
            prev_left_click = left_click;
            return;
        } else if (m_hit == MENUBAR_HIT_ITEM_SETTINGS) {
            windows[2].is_open = true;
            windows[2].is_minimized = false;
            focus_window(2);
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
            if (dock_idx >= 0 && dock_idx < 6 && dock_idx < window_count) {
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
            } else if (dock_idx == 6) { // Trash Icon
                memset(paint_canvas, 0, sizeof(paint_canvas));
            }
            prev_left_click = left_click;
            return;
        }

        // 3. Desktop Drive Icon Double-Click (RatanaOS HD)
        if (mx >= sw - 88 && mx <= sw - 20 && my >= 40 && my <= 110) {
            windows[0].is_open = true; // Finder
            windows[0].is_minimized = false;
            focus_window(0);
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
            if (win->is_open && !win->is_minimized && win->id == 4) { // Paint Studio
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

    // Register 6 macOS Desktop Applications
    create_window(0, "Finder", 40, 48, 480, 260, draw_finder_content, NULL);
    create_window(1, "Terminal", 540, 48, 460, 260, draw_terminal_content, NULL);
    create_window(2, "Activity Monitor", 540, 320, 460, 280, draw_sysmon_content, NULL);
    create_window(3, "Calculator", 40, 320, 240, 280, draw_calc_content, handle_calc_click);
    create_window(4, "Paint Studio", 300, 320, 360, 280, draw_paint_content, handle_paint_click);
    create_window(5, "About This Mac", 272, 160, 480, 320, draw_about_content, NULL);

    // Initial focus on About This Mac dialog
    focus_window(5);
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
