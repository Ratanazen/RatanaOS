#include "../include/icons.h"
#include "../include/gfx.h"

//  Apple Logo Bitmap (14x16) for Menu Bar
static const uint16_t apple_16_bitmap[16] = {
    0x0180, //    ##
    0x00C0, //     ##
    0x0000,
    0x07E0, //   ######
    0x1FF8, //  ##########
    0x3FFC, // ############
    0x3FFE, // #############
    0x3FFE, // #############
    0x3FFE, // #############
    0x3FFC, // ############
    0x1FF8, //  ##########
    0x1FF8, //  ##########
    0x0FF0, //   ########
    0x0E70, //   ###  ###
    0x0C30, //   ##    ##
    0x0000
};

void icon_draw_apple_logo(int x, int y, uint32_t color) {
    for (int r = 0; r < 16; r++) {
        uint16_t row = apple_16_bitmap[r];
        for (int c = 0; c < 16; c++) {
            if (row & (0x8000 >> c)) {
                gfx_draw_pixel(x + c, y + r, color);
            }
        }
    }
}

// 64x64 Large  Apple Emblem for "About This Mac"
void icon_draw_apple_64(int x, int y) {
    // Outer metallic silver badge
    gfx_draw_rounded_rect(x, y, 64, 64, 16, 0x003A3A3C);
    gfx_draw_rounded_rect_outline(x, y, 64, 64, 16, 0x00636366);

    // Scaled centered Apple Logo (28x32)
    for (int r = 0; r < 16; r++) {
        uint16_t row = apple_16_bitmap[r];
        for (int c = 0; c < 16; c++) {
            if (row & (0x8000 >> c)) {
                int px = x + 16 + c * 2;
                int py = y + 16 + r * 2;
                gfx_draw_rect(px, py, 2, 2, COLOR_WHITE);
            }
        }
    }
}

// 1. Finder Dock Icon (48x48)
void icon_draw_finder_48(int x, int y) {
    int s = 48;
    // Outer rounded square
    gfx_draw_rounded_rect(x, y, s, s, 12, 0x000A84FF);

    // Two-tone face: Left Navy Blue, Right Cyan
    gfx_draw_rect(x + 4, y + 4, (s - 8) / 2, s - 8, 0x001B68D1);
    gfx_draw_rect(x + 4 + (s - 8) / 2, y + 4, (s - 8) / 2, s - 8, 0x005AC8FA);

    // Center divider line (Nose)
    int mid_x = x + s / 2;
    gfx_draw_line(mid_x, y + 8, mid_x, y + s - 16, COLOR_WHITE);

    // Eyes
    gfx_draw_rounded_rect(x + 10, y + 14, 6, 10, 2, COLOR_WHITE);
    gfx_draw_rounded_rect(x + s - 16, y + 14, 6, 10, 2, COLOR_WHITE);

    // Smile
    int sy = y + 32;
    gfx_draw_line(x + 10, sy, x + 16, sy + 5, COLOR_WHITE);
    gfx_draw_line(x + 16, sy + 5, x + s - 16, sy + 5, COLOR_WHITE);
    gfx_draw_line(x + s - 16, sy + 5, x + s - 10, sy, COLOR_WHITE);
}

// 2. Terminal Dock Icon (48x48)
void icon_draw_terminal_48(int x, int y) {
    int s = 48;
    gfx_draw_rounded_rect(x, y, s, s, 12, 0x001C1C1E);
    gfx_draw_rounded_rect_outline(x, y, s, s, 12, 0x003A3A3C);

    // Top window bar inside terminal icon
    gfx_draw_rect(x + 4, y + 4, s - 8, 8, 0x002C2C2E);
    gfx_draw_circle(x + 8, y + 8, 2, 0x00FF5F56);
    gfx_draw_circle(x + 14, y + 8, 2, 0x00FFBD2E);
    gfx_draw_circle(x + 20, y + 8, 2, 0x0027C93F);

    // Green `>_` Prompt
    int px = x + 8;
    int py = y + 18;
    gfx_draw_line(px, py, px + 8, py + 6, 0x0030D158);
    gfx_draw_line(px + 8, py + 6, px, py + 12, 0x0030D158);
    gfx_draw_rect(px + 12, py + 10, 8, 3, COLOR_WHITE); // Cursor
}

// 3. Activity Monitor Dock Icon (48x48)
void icon_draw_sysmon_48(int x, int y) {
    int s = 48;
    gfx_draw_rounded_rect(x, y, s, s, 12, 0x002C2C2E);
    gfx_draw_rounded_rect_outline(x, y, s, s, 12, 0x000A84FF);

    // Grid lines
    int mid_y = y + s / 2;
    gfx_draw_line(x + 6, mid_y, x + s - 6, mid_y, 0x003A3A3C);
    gfx_draw_line(x + s / 2, y + 6, x + s / 2, y + s - 6, 0x003A3A3C);

    // Heartbeat Activity Waveform
    int px = x + 6;
    gfx_draw_line(px, mid_y, px + 6, mid_y, 0x0030D158);
    gfx_draw_line(px + 6, mid_y, px + 12, mid_y - 12, 0x0030D158);
    gfx_draw_line(px + 12, mid_y - 12, px + 18, mid_y + 14, 0x0030D158);
    gfx_draw_line(px + 18, mid_y + 14, px + 24, mid_y - 6, 0x0030D158);
    gfx_draw_line(px + 24, mid_y - 6, px + 30, mid_y, 0x0030D158);
    gfx_draw_line(px + 30, mid_y, x + s - 6, mid_y, 0x0030D158);
}

// 4. Calculator Dock Icon (48x48)
void icon_draw_calculator_48(int x, int y) {
    int s = 48;
    gfx_draw_rounded_rect(x, y, s, s, 12, 0x00FF9F0A);

    // Partition lines
    int mid_x = x + s / 2;
    int mid_y = y + s / 2;
    gfx_draw_line(mid_x, y + 6, mid_x, y + s - 6, 0x00E08A00);
    gfx_draw_line(x + 6, mid_y, x + s - 6, mid_y, 0x00E08A00);

    // Math symbols
    gfx_draw_string_transparent(x + 10, y + 8, "+", COLOR_WHITE);
    gfx_draw_string_transparent(mid_x + 10, y + 8, "-", COLOR_WHITE);
    gfx_draw_string_transparent(x + 10, mid_y + 8, "*", COLOR_WHITE);
    gfx_draw_string_transparent(mid_x + 10, mid_y + 8, "=", COLOR_WHITE);
}

// 5. Paint Studio Dock Icon (48x48)
void icon_draw_paint_48(int x, int y) {
    int s = 48;
    gfx_draw_rounded_rect(x, y, s, s, 12, 0x00BF5AF2);

    // White Artist Palette Circle
    gfx_draw_circle(x + s / 2, y + s / 2, 16, COLOR_WHITE);

    // Paint Color Swatches
    gfx_draw_circle(x + s / 2 - 6, y + s / 3 + 2, 3, 0x00FF453A); // Red
    gfx_draw_circle(x + (2 * s) / 3 - 4, y + s / 2 - 4, 3, 0x00FFD60A); // Yellow
    gfx_draw_circle(x + s / 2 - 2, y + (2 * s) / 3 - 4, 3, 0x0030D158); // Green
    gfx_draw_circle(x + s / 3 + 2, y + s / 2 - 2, 3, 0x000A84FF); // Blue
}

// 6. About This Mac Dock Icon (48x48)
void icon_draw_about_48(int x, int y) {
    int s = 48;
    gfx_draw_rounded_rect(x, y, s, s, 12, 0x003A3A3C);
    gfx_draw_rounded_rect_outline(x, y, s, s, 12, 0x00636366);

    // Centered Apple Logo
    icon_draw_apple_logo(x + 16, y + 16, COLOR_WHITE);
}

// 7. Trash Dock Icon (48x48)
void icon_draw_trash_48(int x, int y) {
    int s = 48;
    gfx_draw_rounded_rect(x, y, s, s, 12, 0x0048484A);

    // Lid & Handle
    gfx_draw_rounded_rect(x + 10, y + 8, s - 20, 4, 2, COLOR_LIGHT_GRAY);
    gfx_draw_rect(x + s / 2 - 4, y + 5, 8, 3, COLOR_LIGHT_GRAY);

    // Mesh Basket Body
    gfx_draw_rect(x + 12, y + 13, s - 24, s - 20, 0x00636366);
    gfx_draw_rect_outline(x + 12, y + 13, s - 24, s - 20, COLOR_LIGHT_GRAY);

    // Vertical slats
    gfx_draw_line(x + 18, y + 15, x + 18, y + s - 9, COLOR_LIGHT_GRAY);
    gfx_draw_line(x + 24, y + 15, x + 24, y + s - 9, COLOR_LIGHT_GRAY);
    gfx_draw_line(x + 30, y + 15, x + 30, y + s - 9, COLOR_LIGHT_GRAY);
}

// Desktop Drive Icon (48x48)
void icon_draw_drive_48(int x, int y) {
    // Silver Enclosure
    gfx_draw_rounded_rect(x, y, 48, 36, 6, 0x008E8E93);
    gfx_draw_rounded_rect_outline(x, y, 48, 36, 6, 0x00AEAEB2);

    // Drive slot
    gfx_draw_rect(x + 6, y + 6, 36, 16, 0x00636366);

    // LED Indicator
    gfx_draw_circle(x + 10, y + 28, 2, 0x0030D158); // Green active LED
    gfx_draw_rect(x + 18, y + 27, 24, 3, 0x003A3A3C);
}

// Status Bar Icons (24x24 & Extras)
void icon_draw_wifi(int x, int y, uint32_t color) {
    gfx_draw_line(x, y + 2, x + 4, y, color);
    gfx_draw_line(x + 4, y, x + 8, y + 2, color);

    gfx_draw_line(x + 2, y + 5, x + 4, y + 3, color);
    gfx_draw_line(x + 4, y + 3, x + 6, y + 5, color);

    gfx_draw_pixel(x + 4, y + 7, color);
}

void icon_draw_battery(int x, int y, uint32_t color) {
    gfx_draw_rect_outline(x, y + 2, 16, 9, color);
    gfx_draw_rect(x + 16, y + 5, 2, 3, color);
    gfx_draw_rect(x + 2, y + 4, 11, 5, 0x0030D158); // Green battery
}

void icon_draw_search(int x, int y, uint32_t color) {
    gfx_draw_circle(x + 5, y + 5, 4, color);
    gfx_draw_line(x + 8, y + 8, x + 12, y + 12, color);
}

void icon_draw_control_center(int x, int y, uint32_t color) {
    gfx_draw_rect(x, y + 2, 12, 3, color);
    gfx_draw_rect(x + 2, y + 1, 3, 5, COLOR_WHITE);
    gfx_draw_rect(x, y + 7, 12, 3, color);
    gfx_draw_rect(x + 7, y + 6, 3, 5, COLOR_WHITE);
}
