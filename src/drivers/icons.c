#include "../include/icons.h"
#include "../include/gfx.h"

//  Apple Logo Bitmap (14x16)
static const uint16_t apple_bitmap[16] = {
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

void icon_draw_apple(int x, int y, uint32_t color) {
    for (int r = 0; r < 16; r++) {
        uint16_t row = apple_bitmap[r];
        for (int c = 0; c < 16; c++) {
            if (row & (0x8000 >> c)) {
                gfx_draw_pixel(x + c, y + r, color);
            }
        }
    }
}

// 1. macOS Finder Icon (Blue/Cyan smiling face)
void icon_draw_finder(int x, int y, int size) {
    int s = size;
    // Rounded dark border background
    gfx_draw_rect(x + 2, y, s - 4, s, 0x000A84FF);
    gfx_draw_rect(x, y + 2, s, s - 4, 0x000A84FF);

    // Left face (Darker Blue), Right face (Cyan)
    gfx_draw_rect(x + 3, y + 3, (s - 6) / 2, s - 6, 0x001B68D1);
    gfx_draw_rect(x + 3 + (s - 6) / 2, y + 3, (s - 6) / 2, s - 6, 0x005AC8FA);

    // Two-tone divider line in middle
    int mid_x = x + s / 2;
    gfx_draw_line(mid_x, y + 6, mid_x, y + s - 12, COLOR_WHITE);

    // Left Eye & Right Eye
    gfx_draw_rect(x + s / 4, y + s / 3, 3, 6, COLOR_WHITE);
    gfx_draw_rect(x + (3 * s) / 4 - 3, y + s / 3, 3, 6, COLOR_WHITE);

    // Smile Curve
    int sy = y + (2 * s) / 3;
    gfx_draw_line(x + s / 4, sy, x + s / 3, sy + 3, COLOR_WHITE);
    gfx_draw_line(x + s / 3, sy + 3, x + (2 * s) / 3, sy + 3, COLOR_WHITE);
    gfx_draw_line(x + (2 * s) / 3, sy + 3, x + (3 * s) / 4, sy, COLOR_WHITE);
}

// 2. macOS Terminal Icon
void icon_draw_terminal(int x, int y, int size) {
    int s = size;
    gfx_draw_rect(x + 2, y, s - 4, s, 0x001C1C1E);
    gfx_draw_rect(x, y + 2, s, s - 4, 0x001C1C1E);
    gfx_draw_rect_outline(x + 2, y + 2, s - 4, s - 4, 0x003A3A3C);

    // Top titlebar inside icon
    gfx_draw_rect(x + 3, y + 3, s - 6, 5, 0x002C2C2E);

    // Prompt `>_`
    int px = x + 6;
    int py = y + 12;
    gfx_draw_line(px, py, px + 5, py + 4, 0x0030D158); // Green >
    gfx_draw_line(px + 5, py + 4, px, py + 8, 0x0030D158);
    gfx_draw_rect(px + 8, py + 6, 6, 2, COLOR_WHITE);  // Cursor _
}

// 3. macOS Activity Monitor / Settings Icon
void icon_draw_sysmon(int x, int y, int size) {
    int s = size;
    gfx_draw_rect(x + 2, y, s - 4, s, 0x002C2C2E);
    gfx_draw_rect(x, y + 2, s, s - 4, 0x002C2C2E);
    gfx_draw_rect_outline(x + 2, y + 2, s - 4, s - 4, 0x000A84FF);

    // Grid Lines
    int mid_y = y + s / 2;
    gfx_draw_line(x + 4, mid_y, x + s - 4, mid_y, 0x003A3A3C);
    gfx_draw_line(x + s / 2, y + 4, x + s / 2, y + s - 4, 0x003A3A3C);

    // Pulse wave graph
    int px = x + 4;
    gfx_draw_line(px, mid_y, px + 5, mid_y, 0x0030D158);
    gfx_draw_line(px + 5, mid_y, px + 9, mid_y - 8, 0x0030D158);
    gfx_draw_line(px + 9, mid_y - 8, px + 14, mid_y + 8, 0x0030D158);
    gfx_draw_line(px + 14, mid_y + 8, px + 18, mid_y - 4, 0x0030D158);
    gfx_draw_line(px + 18, mid_y - 4, px + 22, mid_y, 0x0030D158);
    gfx_draw_line(px + 22, mid_y, x + s - 4, mid_y, 0x0030D158);
}

// 4. macOS Calculator Icon
void icon_draw_calculator(int x, int y, int size) {
    int s = size;
    gfx_draw_rect(x + 2, y, s - 4, s, 0x00FF9F0A); // Orange
    gfx_draw_rect(x, y + 2, s, s - 4, 0x00FF9F0A);

    // Grid partition
    int mid_x = x + s / 2;
    int mid_y = y + s / 2;
    gfx_draw_line(mid_x, y + 3, mid_x, y + s - 3, 0x00E08A00);
    gfx_draw_line(x + 3, mid_y, x + s - 3, mid_y, 0x00E08A00);

    // Symbols: +, -, *, =
    gfx_draw_string_transparent(x + 6, y + 4, "+", COLOR_WHITE);
    gfx_draw_string_transparent(x + s / 2 + 6, y + 4, "-", COLOR_WHITE);
    gfx_draw_string_transparent(x + 6, y + s / 2 + 4, "*", COLOR_WHITE);
    gfx_draw_string_transparent(x + s / 2 + 6, y + s / 2 + 4, "=", COLOR_WHITE);
}

// 5. macOS Paint Studio Icon
void icon_draw_paint(int x, int y, int size) {
    int s = size;
    gfx_draw_rect(x + 2, y, s - 4, s, 0x00BF5AF2); // Purple
    gfx_draw_rect(x, y + 2, s, s - 4, 0x00BF5AF2);

    // Palette shape
    gfx_draw_circle(x + s / 2, y + s / 2, s / 3, COLOR_WHITE);

    // Paint spots
    gfx_draw_rect(x + s / 2 - 4, y + s / 3, 3, 3, COLOR_RED);
    gfx_draw_rect(x + (2 * s) / 3 - 3, y + s / 2 - 3, 3, 3, 0x00FFD60A);
    gfx_draw_rect(x + s / 2 - 2, y + (2 * s) / 3 - 4, 3, 3, 0x0030D158);
    gfx_draw_rect(x + s / 3 + 1, y + s / 2 - 2, 3, 3, 0x000A84FF);
}

// 6. macOS About This Mac Emblem Icon
void icon_draw_about(int x, int y, int size) {
    int s = size;
    gfx_draw_rect(x + 2, y, s - 4, s, 0x003A3A3C);
    gfx_draw_rect(x, y + 2, s, s - 4, 0x003A3A3C);
    gfx_draw_rect_outline(x + 2, y + 2, s - 4, s - 4, 0x00636366);

    // Centered Apple logo inside
    icon_draw_apple(x + s / 2 - 8, y + s / 2 - 8, COLOR_WHITE);
}

// 7. macOS Trash Icon
void icon_draw_trash(int x, int y, int size) {
    int s = size;
    gfx_draw_rect(x + 2, y, s - 4, s, 0x0048484A);
    gfx_draw_rect(x, y + 2, s, s - 4, 0x0048484A);

    // Trash can lid
    gfx_draw_rect(x + 6, y + 6, s - 12, 3, COLOR_LIGHT_GRAY);
    gfx_draw_rect(x + s / 2 - 3, y + 4, 6, 2, COLOR_LIGHT_GRAY);

    // Can body
    gfx_draw_rect(x + 8, y + 9, s - 16, s - 14, 0x00636366);
    gfx_draw_rect_outline(x + 8, y + 9, s - 16, s - 14, COLOR_LIGHT_GRAY);

    // Vertical ribbed slats
    gfx_draw_line(x + 12, y + 11, x + 12, y + s - 7, COLOR_LIGHT_GRAY);
    gfx_draw_line(x + s / 2, y + 11, x + s / 2, y + s - 7, COLOR_LIGHT_GRAY);
    gfx_draw_line(x + s - 12, y + 11, x + s - 12, y + s - 7, COLOR_LIGHT_GRAY);
}

// Desktop Drive Icon (RatanaOS HD)
void icon_draw_drive(int x, int y) {
    // Silver enclosure
    gfx_draw_rect(x + 4, y, 32, 24, 0x008E8E93);
    gfx_draw_rect_outline(x + 4, y, 32, 24, 0x00AEAEB2);

    // Drive face & LED
    gfx_draw_rect(x + 8, y + 4, 24, 12, 0x00636366);
    gfx_draw_rect(x + 10, y + 19, 4, 2, 0x0030D158); // Green activity LED
    gfx_draw_rect(x + 18, y + 19, 14, 2, 0x003A3A3C);
}

// Menu Extras: Wi-Fi symbol
void icon_draw_wifi(int x, int y, uint32_t color) {
    // 3 curved arches
    gfx_draw_line(x, y + 2, x + 4, y, color);
    gfx_draw_line(x + 4, y, x + 8, y + 2, color);

    gfx_draw_line(x + 2, y + 5, x + 4, y + 3, color);
    gfx_draw_line(x + 4, y + 3, x + 6, y + 5, color);

    gfx_draw_pixel(x + 4, y + 7, color);
}

// Menu Extras: Battery symbol
void icon_draw_battery(int x, int y, uint32_t color) {
    gfx_draw_rect_outline(x, y + 2, 16, 9, color);
    gfx_draw_rect(x + 16, y + 5, 2, 3, color); // Nipple
    gfx_draw_rect(x + 2, y + 4, 11, 5, 0x0030D158); // Green battery level
}

// Menu Extras: Spotlight search magnifying glass
void icon_draw_search(int x, int y, uint32_t color) {
    gfx_draw_circle(x + 5, y + 5, 4, color);
    gfx_draw_line(x + 8, y + 8, x + 12, y + 12, color);
}

// Menu Extras: Control Center toggles
void icon_draw_control_center(int x, int y, uint32_t color) {
    gfx_draw_rect(x, y + 2, 12, 3, color);
    gfx_draw_rect(x + 2, y + 1, 3, 5, COLOR_WHITE);
    gfx_draw_rect(x, y + 7, 12, 3, color);
    gfx_draw_rect(x + 7, y + 6, 3, 5, COLOR_WHITE);
}
