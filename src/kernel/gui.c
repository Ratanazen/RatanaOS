#include "../include/gui.h"
#include "../include/gfx.h"
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

#define TASKBAR_HEIGHT 38
#define TITLEBAR_HEIGHT 26

static window_t windows[MAX_WINDOWS];
static int window_count = 0;
static bool gui_running = false;
static bool start_menu_open = false;
static bool prev_left_click = false;

// Calculator state
static char calc_display[32] = "0";
static int  calc_op1 = 0;
static char calc_op = 0;
static bool calc_clear_on_next = false;

// Paint canvas state (320x180 pixels canvas)
#define CANVAS_W 320
#define CANVAS_H 180
static uint32_t paint_canvas[CANVAS_W * CANVAS_H];
static uint32_t paint_cur_color = COLOR_ARCH_BLUE;

// Forward Declarations
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
// Application 1: GUI Terminal
// -------------------------------------------------------------
static void draw_terminal_content(window_t* win) {
    int cx = win->x + 8;
    int cy = win->y + TITLEBAR_HEIGHT + 8;
    int cw = win->width - 16;
    int ch = win->height - TITLEBAR_HEIGHT - 16;

    gfx_draw_rect(cx, cy, cw, ch, COLOR_BLACK);
    gfx_draw_rect_outline(cx, cy, cw, ch, COLOR_BORDER);

    gfx_draw_string_transparent(cx + 8, cy + 8, "RatanaOS v2.0 (2026 GUI Edition)", COLOR_GREEN);
    gfx_draw_string_transparent(cx + 8, cy + 28, "Kernel: 32-bit x86 Protected Mode", COLOR_TEXT_MUTED);
    gfx_draw_string_transparent(cx + 8, cy + 48, "Type 'help' for built-in commands.", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(cx + 8, cy + 76, "ratana@os-2026 >", COLOR_ARCH_BLUE);
    gfx_draw_string_transparent(cx + 148, cy + 76, "fetch", COLOR_WHITE);

    gfx_draw_string_transparent(cx + 8, cy + 104, "       /\\       OS:     RatanaOS 2026", COLOR_CYAN);
    gfx_draw_string_transparent(cx + 8, cy + 122, "      /  \\      Kernel: x86 Protected Mode", COLOR_CYAN);
    gfx_draw_string_transparent(cx + 8, cy + 140, "     / /\\ \\     Memory: 128MB PMM / 8MB Heap", COLOR_CYAN);
    gfx_draw_string_transparent(cx + 8, cy + 158, "    / /__\\ \\    GUI:    VBE 32-bit Framebuffer", COLOR_CYAN);
    gfx_draw_string_transparent(cx + 8, cy + 176, "   /_/      \\_\\ Shell:  RatanaSH 2026", COLOR_CYAN);

    // Blinking cursor
    uint32_t ticks = timer_get_ticks();
    if ((ticks / 50) % 2 == 0) {
        gfx_draw_rect(cx + 8, cy + 204, 8, 16, COLOR_TEXT_WHITE);
    }
}

// -------------------------------------------------------------
// Application 2: System Monitor
// -------------------------------------------------------------
static void draw_sysmon_content(window_t* win) {
    int cx = win->x + 8;
    int cy = win->y + TITLEBAR_HEIGHT + 8;

    heap_stats_t hstats = heap_get_stats();
    size_t free_pgs = pmm_get_free_pages();
    size_t total_pgs = pmm_get_total_pages();
    cpu_info_t cpu = cpuid_get_info();
    uint32_t uptime_sec = timer_get_ticks() / 100;

    gfx_draw_string_transparent(cx + 8, cy + 8, "Hardware & Resource Overview", COLOR_ARCH_BLUE);

    // CPU Info
    char cpu_str[64];
    strcpy(cpu_str, "CPU: ");
    strcat(cpu_str, cpu.brand[0] ? cpu.brand : "x86 Compatible Processor");
    gfx_draw_string_transparent(cx + 8, cy + 30, cpu_str, COLOR_TEXT_WHITE);

    // Uptime
    char upt_str[32];
    strcpy(upt_str, "Uptime: ");
    char num_buf[16];
    utoa(uptime_sec, num_buf, 10);
    strcat(upt_str, num_buf);
    strcat(upt_str, " seconds");
    gfx_draw_string_transparent(cx + 8, cy + 50, upt_str, COLOR_TEXT_MUTED);

    // Physical RAM Bar
    gfx_draw_string_transparent(cx + 8, cy + 78, "Physical Memory (128 MB)", COLOR_TEXT_WHITE);
    int bar_w = win->width - 32;
    int used_pgs = (int)(total_pgs - free_pgs);
    int pmm_fill = total_pgs > 0 ? (used_pgs * bar_w) / (int)total_pgs : 0;
    gfx_draw_rect(cx + 8, cy + 98, bar_w, 18, COLOR_PANEL_DARK);
    gfx_draw_rect(cx + 8, cy + 98, pmm_fill < 10 ? 10 : pmm_fill, 18, COLOR_GREEN);
    gfx_draw_rect_outline(cx + 8, cy + 98, bar_w, 18, COLOR_BORDER);

    // Kernel Heap Bar
    gfx_draw_string_transparent(cx + 8, cy + 128, "Kernel Dynamic Heap (8192 KB)", COLOR_TEXT_WHITE);
    int heap_fill = hstats.total_size > 0 ? (int)((hstats.used_size * bar_w) / hstats.total_size) : 0;
    gfx_draw_rect(cx + 8, cy + 148, bar_w, 18, COLOR_PANEL_DARK);
    gfx_draw_rect(cx + 8, cy + 148, heap_fill < 10 ? 10 : heap_fill, 18, COLOR_ORANGE);
    gfx_draw_rect_outline(cx + 8, cy + 148, bar_w, 18, COLOR_BORDER);

    // Heap stats summary
    char heap_txt[64];
    strcpy(heap_txt, "Heap Used: ");
    utoa((uint32_t)(hstats.used_size / 1024), num_buf, 10);
    strcat(heap_txt, num_buf);
    strcat(heap_txt, " KB | Active Blocks: ");
    utoa((uint32_t)hstats.num_allocations, num_buf, 10);
    strcat(heap_txt, num_buf);
    gfx_draw_string_transparent(cx + 8, cy + 174, heap_txt, COLOR_TEXT_MUTED);
}

// -------------------------------------------------------------
// Application 3: Calculator
// -------------------------------------------------------------
static const char* calc_buttons[4][4] = {
    {"7", "8", "9", "/"},
    {"4", "5", "6", "*"},
    {"1", "2", "3", "-"},
    {"C", "0", "=", "+"}
};

static void draw_calc_content(window_t* win) {
    int cx = win->x + 12;
    int cy = win->y + TITLEBAR_HEIGHT + 12;

    // Display box
    gfx_draw_rect(cx, cy, 216, 36, COLOR_BLACK);
    gfx_draw_rect_outline(cx, cy, 216, 36, COLOR_BORDER);
    gfx_draw_string_transparent(cx + 10, cy + 10, calc_display, COLOR_GREEN);

    // Buttons
    int btn_w = 48;
    int btn_h = 32;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int bx = cx + c * (btn_w + 8);
            int by = cy + 46 + r * (btn_h + 8);
            uint32_t bg = (c == 3 || (r == 3 && c == 2)) ? COLOR_ARCH_BLUE : COLOR_PANEL_LIGHT;
            if (r == 3 && c == 0) bg = COLOR_CLOSE_RED;
            gfx_draw_rect(bx, by, btn_w, btn_h, bg);
            gfx_draw_rect_outline(bx, by, btn_w, btn_h, COLOR_BORDER);
            gfx_draw_string_transparent(bx + 18, by + 8, calc_buttons[r][c], COLOR_TEXT_WHITE);
        }
    }
}

static void handle_calc_click(window_t* win, int rel_x, int rel_y, int btn) {
    (void)win;
    (void)btn;
    int cx = 12;
    int cy = TITLEBAR_HEIGHT + 12 + 46;
    int btn_w = 48;
    int btn_h = 32;

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
                } else if (strcmp(label, "=") == 0 && calc_op != 0) {
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
                return;
            }
        }
    }
}

// -------------------------------------------------------------
// Application 4: Paint Canvas
// -------------------------------------------------------------
static const uint32_t paint_colors[8] = {
    COLOR_WHITE, COLOR_RED, COLOR_GREEN, COLOR_BLUE,
    COLOR_YELLOW, COLOR_CYAN, COLOR_ORANGE, COLOR_BLACK
};

static void draw_paint_content(window_t* win) {
    int cx = win->x + 8;
    int cy = win->y + TITLEBAR_HEIGHT + 8;

    // Palette bar
    for (int i = 0; i < 8; i++) {
        int px = cx + i * 28;
        int py = cy;
        gfx_draw_rect(px, py, 22, 22, paint_colors[i]);
        gfx_draw_rect_outline(px, py, 22, 22, (paint_cur_color == paint_colors[i]) ? COLOR_WHITE : COLOR_BORDER);
    }

    // Canvas drawing area
    int can_y = cy + 30;
    gfx_draw_rect_outline(cx - 1, can_y - 1, CANVAS_W + 2, CANVAS_H + 2, COLOR_BORDER);

    for (int y = 0; y < CANVAS_H; y++) {
        for (int x = 0; x < CANVAS_W; x++) {
            uint32_t col = paint_canvas[y * CANVAS_W + x];
            if (col != 0) {
                gfx_draw_pixel(cx + x, can_y + y, col);
            } else {
                gfx_draw_pixel(cx + x, can_y + y, COLOR_WHITE);
            }
        }
    }
}

static void handle_paint_click(window_t* win, int rel_x, int rel_y, int btn) {
    (void)win;
    (void)btn;
    int cx = 8;
    int cy = TITLEBAR_HEIGHT + 8;

    // Check color palette click
    if (rel_y >= cy && rel_y < cy + 22) {
        for (int i = 0; i < 8; i++) {
            int px = cx + i * 28;
            if (rel_x >= px && rel_x < px + 22) {
                paint_cur_color = paint_colors[i];
                return;
            }
        }
    }

    // Check canvas brush click
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
// Application 5: About RatanaOS
// -------------------------------------------------------------
static void draw_about_content(window_t* win) {
    int cx = win->x + 16;
    int cy = win->y + TITLEBAR_HEIGHT + 16;

    gfx_draw_string_transparent(cx, cy, "RatanaOS 2026", COLOR_ARCH_BLUE);
    gfx_draw_string_transparent(cx, cy + 22, "Full-Feature 32-bit x86 Operating System", COLOR_TEXT_WHITE);
    gfx_draw_string_transparent(cx, cy + 44, "Author:   Ratanazen", COLOR_TEXT_MUTED);
    gfx_draw_string_transparent(cx, cy + 64, "Edition:  2026 High-Resolution GUI", COLOR_TEXT_MUTED);
    gfx_draw_string_transparent(cx, cy + 84, "Graphics: VBE 32-bit Framebuffer", COLOR_TEXT_MUTED);
    gfx_draw_string_transparent(cx, cy + 104, "Drivers:  GDT, IDT, PIC, PIT, PS/2 Mouse/Kbd", COLOR_TEXT_MUTED);
    gfx_draw_string_transparent(cx, cy + 124, "          CMOS RTC, PC Speaker, PCI, CPUID", COLOR_TEXT_MUTED);
    gfx_draw_string_transparent(cx, cy + 150, "Enjoy computing on RatanaOS!", COLOR_GREEN);
}

// -------------------------------------------------------------
// Window Rendering & Frame Decorator
// -------------------------------------------------------------
static void draw_window(window_t* win) {
    if (!win->is_open || win->is_minimized) return;

    // Window shadow
    gfx_draw_rect(win->x + 4, win->y + 4, win->width, win->height, COLOR_BLACK);

    // Main window background
    gfx_draw_rect(win->x, win->y, win->width, win->height, COLOR_PANEL_DARK);
    gfx_draw_rect_outline(win->x, win->y, win->width, win->height, win->is_active ? COLOR_ARCH_BLUE : COLOR_BORDER);

    // Titlebar
    uint32_t tb_color = win->is_active ? COLOR_PANEL_LIGHT : COLOR_BG_DARK;
    gfx_draw_rect(win->x + 1, win->y + 1, win->width - 2, TITLEBAR_HEIGHT - 1, tb_color);
    gfx_draw_string_transparent(win->x + 10, win->y + 5, win->title, win->is_active ? COLOR_TEXT_WHITE : COLOR_TEXT_MUTED);

    // Minimize Button [_]
    int min_btn_x = win->x + win->width - 46;
    gfx_draw_rect(min_btn_x, win->y + 5, 16, 16, COLOR_PANEL_DARK);
    gfx_draw_rect_outline(min_btn_x, win->y + 5, 16, 16, COLOR_BORDER);
    gfx_draw_rect(min_btn_x + 3, win->y + 16, 10, 2, COLOR_MIN_YELLOW);

    // Close Button [X]
    int close_btn_x = win->x + win->width - 24;
    gfx_draw_rect(close_btn_x, win->y + 5, 16, 16, COLOR_CLOSE_RED);
    gfx_draw_rect_outline(close_btn_x, win->y + 5, 16, 16, COLOR_BORDER);
    gfx_draw_string_transparent(close_btn_x + 4, win->y + 5, "x", COLOR_WHITE);

    // Draw application contents
    if (win->draw_content) {
        win->draw_content(win);
    }
}

// -------------------------------------------------------------
// Taskbar & Desktop Wallpaper
// -------------------------------------------------------------
static void draw_desktop_and_taskbar(void) {
    int sw = gfx_get_width();
    int sh = gfx_get_height();

    // 1. Wallpaper Gradient
    gfx_draw_gradient_v(0, 0, sw, sh - TASKBAR_HEIGHT, COLOR_DARK_BLUE, COLOR_BG_DARK);

    // RatanaOS Desktop Watermark
    gfx_draw_string_transparent(sw - 200, sh - TASKBAR_HEIGHT - 30, "RatanaOS 2026", COLOR_BORDER);

    // 2. Windows
    for (int i = 0; i < window_count; i++) {
        draw_window(&windows[i]);
    }

    // 3. Taskbar Panel
    int ty = sh - TASKBAR_HEIGHT;
    gfx_draw_rect(0, ty, sw, TASKBAR_HEIGHT, COLOR_PANEL_DARK);
    gfx_draw_rect(0, ty, sw, 1, COLOR_BORDER);

    // Start Button
    uint32_t start_color = start_menu_open ? COLOR_ARCH_BLUE : COLOR_PANEL_LIGHT;
    gfx_draw_rect(8, ty + 4, 100, 30, start_color);
    gfx_draw_rect_outline(8, ty + 4, 100, 30, COLOR_BORDER);
    gfx_draw_string_transparent(16, ty + 10, "RatanaOS", COLOR_TEXT_WHITE);

    // Window Tabs in Taskbar
    int tab_x = 116;
    for (int i = 0; i < window_count; i++) {
        if (!windows[i].is_open) continue;
        uint32_t tab_bg = windows[i].is_active ? COLOR_PANEL_LIGHT : COLOR_BG_DARK;
        gfx_draw_rect(tab_x, ty + 4, 120, 30, tab_bg);
        gfx_draw_rect_outline(tab_x, ty + 4, 120, 30, windows[i].is_active ? COLOR_ARCH_BLUE : COLOR_BORDER);

        char tab_title[14];
        strncpy(tab_title, windows[i].title, 12);
        tab_title[12] = '\0';
        gfx_draw_string_transparent(tab_x + 8, ty + 10, tab_title, COLOR_TEXT_WHITE);
        tab_x += 126;
    }

    // Taskbar RAM Indicator Widget
    heap_stats_t hstats = heap_get_stats();
    char ram_txt[32];
    strcpy(ram_txt, "RAM: ");
    char ram_n[16];
    utoa((uint32_t)(hstats.used_size / 1024), ram_n, 10);
    strcat(ram_txt, ram_n);
    strcat(ram_txt, "KB");
    gfx_draw_rect(sw - 280, ty + 6, 95, 26, COLOR_BG_DARK);
    gfx_draw_rect_outline(sw - 280, ty + 6, 95, 26, COLOR_BORDER);
    gfx_draw_string_transparent(sw - 272, ty + 11, ram_txt, COLOR_GREEN);

    // Taskbar RTC 2026 Clock Widget
    rtc_time_t t = rtc_get_time();
    char time_str[32];
    char num[16];
    utoa(t.year, num, 10);
    strcpy(time_str, num);
    strcat(time_str, "-");
    utoa(t.month, num, 10);
    if (t.month < 10) strcat(time_str, "0");
    strcat(time_str, num);
    strcat(time_str, "-");
    utoa(t.day, num, 10);
    if (t.day < 10) strcat(time_str, "0");
    strcat(time_str, num);
    strcat(time_str, " ");
    utoa(t.hour, num, 10);
    if (t.hour < 10) strcat(time_str, "0");
    strcat(time_str, num);
    strcat(time_str, ":");
    utoa(t.minute, num, 10);
    if (t.minute < 10) strcat(time_str, "0");
    strcat(time_str, num);

    gfx_draw_rect(sw - 175, ty + 6, 165, 26, COLOR_BG_DARK);
    gfx_draw_rect_outline(sw - 175, ty + 6, 165, 26, COLOR_BORDER);
    gfx_draw_string_transparent(sw - 167, ty + 11, time_str, COLOR_YELLOW);

    // Start Menu Popup
    if (start_menu_open) {
        int sm_w = 200;
        int sm_h = 190;
        int sm_y = ty - sm_h - 4;
        gfx_draw_rect(8, sm_y, sm_w, sm_h, COLOR_PANEL_DARK);
        gfx_draw_rect_outline(8, sm_y, sm_w, sm_h, COLOR_ARCH_BLUE);

        gfx_draw_string_transparent(16, sm_y + 10, "Applications", COLOR_ARCH_BLUE);
        gfx_draw_rect(16, sm_y + 28, sm_w - 32, 1, COLOR_BORDER);

        const char* menu_items[] = {
            "> Terminal Console",
            "> System Monitor",
            "> Calculator",
            "> Paint Canvas",
            "> About RatanaOS",
            "[X] Exit to CLI"
        };

        for (int i = 0; i < 6; i++) {
            gfx_draw_string_transparent(16, sm_y + 36 + i * 24, menu_items[i],
                                       (i == 5) ? COLOR_CLOSE_RED : COLOR_TEXT_WHITE);
        }
    }
}

// -------------------------------------------------------------
// Mouse & GUI Event Processing
// -------------------------------------------------------------
static void process_gui_events(void) {
    int mx = mouse_get_x();
    int my = mouse_get_y();
    bool left_click = mouse_is_left_clicked();
    bool click_down = left_click && !prev_left_click;
    int sh = gfx_get_height();

    // Check Window Dragging
    for (int i = window_count - 1; i >= 0; i--) {
        if (windows[i].is_dragging) {
            if (left_click) {
                windows[i].x = mx - windows[i].drag_offset_x;
                windows[i].y = my - windows[i].drag_offset_y;
                if (windows[i].x < 0) windows[i].x = 0;
                if (windows[i].y < 0) windows[i].y = 0;
            } else {
                windows[i].is_dragging = false;
            }
            break;
        }
    }

    if (click_down) {
        // 1. Check Start Menu Click
        if (start_menu_open) {
            int sm_y = sh - TASKBAR_HEIGHT - 190 - 4;
            if (mx >= 8 && mx <= 208 && my >= sm_y && my <= sh - TASKBAR_HEIGHT) {
                int item = (my - (sm_y + 36)) / 24;
                if (item >= 0 && item < window_count) {
                    windows[item].is_open = true;
                    windows[item].is_minimized = false;
                    focus_window(item);
                } else if (item == 5) {
                    gui_exit();
                    return;
                }
                start_menu_open = false;
                prev_left_click = left_click;
                return;
            } else {
                start_menu_open = false;
            }
        }

        // 2. Check Start Button Click
        if (mx >= 8 && mx <= 108 && my >= sh - TASKBAR_HEIGHT + 4 && my <= sh - 4) {
            start_menu_open = !start_menu_open;
            prev_left_click = left_click;
            return;
        }

        // 3. Check Taskbar Window Tabs Click
        int tab_x = 116;
        for (int i = 0; i < window_count; i++) {
            if (!windows[i].is_open) continue;
            if (mx >= tab_x && mx <= tab_x + 120 && my >= sh - TASKBAR_HEIGHT + 4 && my <= sh - 4) {
                if (windows[i].is_minimized) {
                    windows[i].is_minimized = false;
                    focus_window(i);
                } else if (windows[i].is_active) {
                    windows[i].is_minimized = true;
                } else {
                    focus_window(i);
                }
                prev_left_click = left_click;
                return;
            }
            tab_x += 126;
        }

        // 4. Check Window Titlebars & Controls (Z-order: top to bottom)
        for (int i = window_count - 1; i >= 0; i--) {
            window_t* win = &windows[i];
            if (!win->is_open || win->is_minimized) continue;

            // Inside window bounding box
            if (mx >= win->x && mx < win->x + win->width && my >= win->y && my < win->y + win->height) {
                focus_window(i);

                // Inside titlebar
                if (my < win->y + TITLEBAR_HEIGHT) {
                    // Close button
                    if (mx >= win->x + win->width - 24 && mx <= win->x + win->width - 8) {
                        win->is_open = false;
                    }
                    // Minimize button
                    else if (mx >= win->x + win->width - 46 && mx <= win->x + win->width - 30) {
                        win->is_minimized = true;
                    }
                    // Start dragging
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

    // Paint continuous drag drawing
    if (left_click) {
        for (int i = window_count - 1; i >= 0; i--) {
            window_t* win = &windows[i];
            if (win->is_open && !win->is_minimized && win->id == 3) { // Paint Canvas app
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
// GUI Life Cycle & Main Loop
// -------------------------------------------------------------
void gui_init(multiboot_info_t* mbi) {
    gfx_init(mbi);
    mouse_init();
    mouse_set_bounds(gfx_get_width(), gfx_get_height());

    window_count = 0;
    memset(paint_canvas, 0, sizeof(paint_canvas));

    // Register all Desktop Applications
    create_window(0, "Terminal Console", 50, 50, 480, 260, draw_terminal_content, NULL);
    create_window(1, "System Monitor", 550, 50, 420, 240, draw_sysmon_content, NULL);
    create_window(2, "Calculator", 50, 330, 240, 270, draw_calc_content, handle_calc_click);
    create_window(3, "Paint Canvas", 310, 330, 340, 270, draw_paint_content, handle_paint_click);
    create_window(4, "About RatanaOS", 670, 330, 300, 240, draw_about_content, NULL);

    // Initial focus on Terminal
    focus_window(0);
}

void gui_start(void) {
    gui_running = true;

    while (gui_running) {
        // Check for keyboard exit (Escape key = scancode 27)
        if (keyboard_has_key()) {
            char k = keyboard_getchar();
            if (k == 27) { // ESC
                gui_exit();
                break;
            }
        }

        // Process mouse & window events
        process_gui_events();

        // Render desktop, windows, and widgets to backbuffer
        draw_desktop_and_taskbar();

        // Draw hardware mouse cursor on top
        mouse_draw_cursor(mouse_get_x(), mouse_get_y());

        // Swap backbuffer to front screen (60 FPS smooth tear-free)
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
