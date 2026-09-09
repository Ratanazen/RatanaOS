#include "../include/icons.h"
#include "../include/gfx.h"
#include "../include/icons_assets.h"
#include "../include/dock.h"

static icon_config_t current_icon_config = {
    .theme = ICON_THEME_WHITESUR,
    .dock_icon_size = 48,
    .dock_spacing = 6,
    .dock_magnification = true,
    .show_desktop_icons = true,
    .desktop_icon_size = 48,
    .show_icon_labels = true,
};

void icon_config_init(void) {
    icon_config_reset_defaults();
}

void icon_config_get(icon_config_t* cfg) {
    if (cfg) {
        *cfg = current_icon_config;
    }
}

void icon_config_set(const icon_config_t* cfg) {
    if (!cfg) return;
    if (cfg->theme >= 0 && cfg->theme < ICON_THEME_COUNT) {
        current_icon_config.theme = cfg->theme;
    }
    if (cfg->dock_icon_size >= 32 && cfg->dock_icon_size <= 64) {
        current_icon_config.dock_icon_size = cfg->dock_icon_size;
    }
    if (cfg->dock_spacing >= 2 && cfg->dock_spacing <= 16) {
        current_icon_config.dock_spacing = cfg->dock_spacing;
    }
    current_icon_config.dock_magnification = cfg->dock_magnification;
    current_icon_config.show_desktop_icons = cfg->show_desktop_icons;
    if (cfg->desktop_icon_size >= 32 && cfg->desktop_icon_size <= 64) {
        current_icon_config.desktop_icon_size = cfg->desktop_icon_size;
    }
    current_icon_config.show_icon_labels = cfg->show_icon_labels;
    icon_config_apply();
}

void icon_config_reset_defaults(void) {
    current_icon_config.theme = ICON_THEME_WHITESUR;
    current_icon_config.dock_icon_size = 48;
    current_icon_config.dock_spacing = 6;
    current_icon_config.dock_magnification = true;
    current_icon_config.show_desktop_icons = true;
    current_icon_config.desktop_icon_size = 48;
    current_icon_config.show_icon_labels = true;
    icon_config_apply();
}

void icon_config_apply(void) {
    dock_set_icon_size(current_icon_config.dock_icon_size);
    dock_set_spacing(current_icon_config.dock_spacing);
    dock_set_magnification(current_icon_config.dock_magnification);
}

void icon_set_theme(icon_theme_id_t theme) {
    if (theme >= 0 && theme < ICON_THEME_COUNT) {
        current_icon_config.theme = theme;
    }
}

icon_theme_id_t icon_get_theme(void) {
    return current_icon_config.theme;
}

void icon_theme_next(void) {
    current_icon_config.theme = (icon_theme_id_t)((current_icon_config.theme + 1) % ICON_THEME_COUNT);
}

const char* icon_get_theme_name(void) {
    return icons_theme_get_name(current_icon_config.theme);
}

// -------------------------------------------------------------
// 32-bit ARGB High-Performance Alpha Icon Blitter
// -------------------------------------------------------------
void gfx_draw_icon_rgba(int x, int y, int w, int h, const uint32_t* pixels) {
    if (!pixels) return;
    int sw = gfx_get_width();
    int sh = gfx_get_height();

    for (int cy = 0; cy < h; cy++) {
        int dst_y = y + cy;
        if (dst_y < 0 || dst_y >= sh) continue;

        for (int cx = 0; cx < w; cx++) {
            int dst_x = x + cx;
            if (dst_x < 0 || dst_x >= sw) continue;

            uint32_t src = pixels[cy * w + cx];
            uint8_t a = (src >> 24) & 0xFF;
            if (a == 0) continue; // 100% transparent

            if (a == 255) {
                gfx_draw_pixel(dst_x, dst_y, src & 0x00FFFFFF);
            } else {
                gfx_draw_pixel_alpha(dst_x, dst_y, src & 0x00FFFFFF, a);
            }
        }
    }
}

// -------------------------------------------------------------
// 32-bit ARGB Scaled Blitter (Arbitrary dimensions with Alpha)
// -------------------------------------------------------------
void gfx_draw_icon_rgba_scaled(int x, int y, int target_w, int target_h, int src_w, int src_h, const uint32_t* pixels) {
    if (!pixels || target_w <= 0 || target_h <= 0 || src_w <= 0 || src_h <= 0) return;
    int sw = gfx_get_width();
    int sh = gfx_get_height();

    if (target_w == src_w && target_h == src_h) {
        gfx_draw_icon_rgba(x, y, target_w, target_h, pixels);
        return;
    }

    uint32_t step_x = ((uint32_t)src_w << 16) / (uint32_t)target_w;
    uint32_t step_y = ((uint32_t)src_h << 16) / (uint32_t)target_h;

    for (int cy = 0; cy < target_h; cy++) {
        int dst_y = y + cy;
        if (dst_y < 0 || dst_y >= sh) continue;

        uint32_t src_y_fp = cy * step_y;
        int src_y = src_y_fp >> 16;
        if (src_y >= src_h) src_y = src_h - 1;

        const uint32_t* row_pixels = &pixels[src_y * src_w];

        for (int cx = 0; cx < target_w; cx++) {
            int dst_x = x + cx;
            if (dst_x < 0 || dst_x >= sw) continue;

            uint32_t src_x_fp = cx * step_x;
            int src_x = src_x_fp >> 16;
            if (src_x >= src_w) src_x = src_w - 1;

            uint32_t src = row_pixels[src_x];
            uint8_t a = (src >> 24) & 0xFF;
            if (a == 0) continue;

            if (a == 255) {
                gfx_draw_pixel(dst_x, dst_y, src & 0x00FFFFFF);
            } else {
                gfx_draw_pixel_alpha(dst_x, dst_y, src & 0x00FFFFFF, a);
            }
        }
    }
}

static bool try_draw_theme_icon(icon_id_t id, int x, int y, int size) {
    if (current_icon_config.theme == ICON_THEME_VECTOR) return false;
    const uint32_t* data = icons_asset_get(current_icon_config.theme, id);
    if (!data) return false;
    int src_size = (id == ICON_ID_ABOUT_64) ? ICON_SIZE_64 : ICON_SIZE_48;
    if (size == src_size) {
        gfx_draw_icon_rgba(x, y, size, size, data);
    } else {
        gfx_draw_icon_rgba_scaled(x, y, size, size, src_size, src_size, data);
    }
    return true;
}

// -------------------------------------------------------------
// Helper: Draw Squircle Base with Vertical Gradient & Highlight
// -------------------------------------------------------------
static void draw_squircle_base(int x, int y, int s, int r, uint32_t top_col, uint32_t bot_col, uint32_t border_col) {
    uint8_t r1 = (top_col >> 16) & 0xFF, g1 = (top_col >> 8) & 0xFF, b1 = top_col & 0xFF;
    uint8_t r2 = (bot_col >> 16) & 0xFF, g2 = (bot_col >> 8) & 0xFF, b2 = bot_col & 0xFF;

    for (int cy = 0; cy < s; cy++) {
        uint8_t cr = (uint8_t)(r1 + ((r2 - r1) * cy) / (s - 1));
        uint8_t cg = (uint8_t)(g1 + ((g2 - g1) * cy) / (s - 1));
        uint8_t cb = (uint8_t)(b1 + ((b2 - b1) * cy) / (s - 1));
        uint32_t col = (cr << 16) | (cg << 8) | cb;

        // Clip rounded corners
        for (int cx = 0; cx < s; cx++) {
            bool inside = true;
            if (cx < r && cy < r) {
                int dx = cx - r, dy = cy - r;
                if (dx * dx + dy * dy > r * r) inside = false;
            } else if (cx >= s - r && cy < r) {
                int dx = cx - (s - r - 1), dy = cy - r;
                if (dx * dx + dy * dy > r * r) inside = false;
            } else if (cx < r && cy >= s - r) {
                int dx = cx - r, dy = cy - (s - r - 1);
                if (dx * dx + dy * dy > r * r) inside = false;
            } else if (cx >= s - r && cy >= s - r) {
                int dx = cx - (s - r - 1), dy = cy - (s - r - 1);
                if (dx * dx + dy * dy > r * r) inside = false;
            }

            if (inside) {
                gfx_draw_pixel(x + cx, y + cy, col);
            }
        }
    }

    // Top rim specular highlight line
    gfx_draw_line(x + r, y + 1, x + s - r - 1, y + 1, 0x00FFFFFF);

    // Outer subtle border
    gfx_draw_rounded_rect_outline(x, y, s, s, r, border_col);
}

// -------------------------------------------------------------
//  Apple Logo Bitmap (14x16) for Menu Bar & Extras
// -------------------------------------------------------------
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

// -------------------------------------------------------------
// 64x64 Large  Apple Emblem for "About This Mac" Dialog
// -------------------------------------------------------------
void icon_draw_apple_64(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_ABOUT_64, x, y, 64)) return;

    // Outer metallic space gray badge
    draw_squircle_base(x, y, 64, 16, 0x0048484A, 0x00242426, 0x00636366);
    gfx_draw_rounded_rect_outline(x + 1, y + 1, 62, 62, 15, 0x001C1C1E);

    // Centered embossed Apple Logo (28x32) with drop shadow
    for (int r = 0; r < 16; r++) {
        uint16_t row = apple_16_bitmap[r];
        for (int c = 0; c < 16; c++) {
            if (row & (0x8000 >> c)) {
                int px = x + 16 + c * 2;
                int py = y + 16 + r * 2;
                // Subtle drop shadow
                gfx_draw_rect(px + 1, py + 1, 2, 2, 0x00101012);
                // Specular face
                gfx_draw_rect(px, py, 2, 2, COLOR_WHITE);
            }
        }
    }
}

// -------------------------------------------------------------
// 1. Finder Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_finder_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_FINDER, x, y, 48)) return;

    int s = 48;
    int r = 11;

    // Draw Left Half (Deep Blue Gradient) and Right Half (Sky Cyan Gradient)
    for (int cy = 0; cy < s; cy++) {
        uint8_t l_r = (uint8_t)(0x2E - (0x13 * cy) / (s - 1));
        uint8_t l_g = (uint8_t)(0x7C - (0x21 * cy) / (s - 1));
        uint8_t l_b = (uint8_t)(0xF6 - (0x2F * cy) / (s - 1));
        uint32_t col_left = (l_r << 16) | (l_g << 8) | l_b;

        uint8_t r_r = (uint8_t)(0x68 - (0x23 * cy) / (s - 1));
        uint8_t r_g = (uint8_t)(0xD0 - (0x22 * cy) / (s - 1));
        uint8_t r_b = (uint8_t)(0xFB - (0x11 * cy) / (s - 1));
        uint32_t col_right = (r_r << 16) | (r_g << 8) | r_b;

        for (int cx = 0; cx < s; cx++) {
            bool inside = true;
            if (cx < r && cy < r) {
                int dx = cx - r, dy = cy - r;
                if (dx * dx + dy * dy > r * r) inside = false;
            } else if (cx >= s - r && cy < r) {
                int dx = cx - (s - r - 1), dy = cy - r;
                if (dx * dx + dy * dy > r * r) inside = false;
            } else if (cx < r && cy >= s - r) {
                int dx = cx - r, dy = cy - (s - r - 1);
                if (dx * dx + dy * dy > r * r) inside = false;
            } else if (cx >= s - r && cy >= s - r) {
                int dx = cx - (s - r - 1), dy = cy - (s - r - 1);
                if (dx * dx + dy * dy > r * r) inside = false;
            }

            if (inside) {
                gfx_draw_pixel(x + cx, y + cy, (cx < s / 2) ? col_left : col_right);
            }
        }
    }

    // Top highlight & border
    gfx_draw_line(x + r, y + 1, x + s - r - 1, y + 1, 0x00FFFFFF);
    gfx_draw_rounded_rect_outline(x, y, s, s, r, 0x001B5BC7);

    // Center Nose Divider Line (Curved in the middle)
    int mid_x = x + 24;
    gfx_draw_line(mid_x, y + 6, mid_x, y + 16, COLOR_WHITE);
    gfx_draw_line(mid_x, y + 16, mid_x - 3, y + 24, COLOR_WHITE);
    gfx_draw_line(mid_x - 3, y + 24, mid_x + 1, y + 27, COLOR_WHITE);
    gfx_draw_line(mid_x + 1, y + 27, mid_x, y + 31, COLOR_WHITE);

    // Eyes: Stylized vertical capsules
    gfx_draw_rounded_rect(x + 12, y + 14, 5, 10, 2, 0x00133E8D);
    gfx_draw_rounded_rect_outline(x + 12, y + 14, 5, 10, 2, COLOR_WHITE);
    gfx_draw_rect(x + 13, y + 15, 2, 3, COLOR_WHITE); // Gleam

    gfx_draw_rounded_rect(x + 31, y + 14, 5, 10, 2, 0x00133E8D);
    gfx_draw_rounded_rect_outline(x + 31, y + 14, 5, 10, 2, COLOR_WHITE);
    gfx_draw_rect(x + 32, y + 15, 2, 3, COLOR_WHITE); // Gleam

    // Smile
    gfx_draw_line(x + 10, y + 33, x + 14, y + 38, COLOR_WHITE);
    gfx_draw_line(x + 14, y + 38, x + 24, y + 40, COLOR_WHITE);
    gfx_draw_line(x + 24, y + 40, x + 34, y + 38, COLOR_WHITE);
    gfx_draw_line(x + 34, y + 38, x + 38, y + 33, COLOR_WHITE);

    gfx_draw_line(x + 10, y + 34, x + 14, y + 39, 0x001B5BC7);
    gfx_draw_line(x + 14, y + 39, x + 34, y + 39, 0x001B5BC7);
    gfx_draw_line(x + 34, y + 39, x + 38, y + 34, 0x001B5BC7);
}

// -------------------------------------------------------------
// 2. Launchpad Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_launchpad_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_LAUNCHPAD, x, y, 48)) return;

    draw_squircle_base(x, y, 48, 11, 0x003A3A3E, 0x001A1A1E, 0x00121214);

    static const uint32_t dot_colors[3][3] = {
        {0x00FF3B30, 0x00FF9500, 0x00FFCC00},
        {0x0034C759, 0x0000C7BE, 0x00007AFF},
        {0x00AF52DE, 0x00FF2D55, 0x00E5E5EA}
    };

    int dot_x[3] = {x + 10, x + 20, x + 30};
    int dot_y[3] = {y + 10, y + 20, y + 30};

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            int cx = dot_x[c] + 4;
            int cy = dot_y[r] + 4;
            gfx_draw_circle(cx + 1, cy + 1, 4, 0x00101012);
            gfx_draw_circle(cx, cy, 4, dot_colors[r][c]);
            gfx_draw_pixel(cx - 1, cy - 1, COLOR_WHITE);
        }
    }
}

// -------------------------------------------------------------
// 3. Safari Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_safari_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_SAFARI, x, y, 48)) return;

    draw_squircle_base(x, y, 48, 11, 0x002196F3, 0x000D47A1, 0x000A3880);

    int cx = x + 24;
    int cy = y + 24;

    gfx_draw_circle(cx, cy, 18, 0x001565C0);
    gfx_draw_line(cx, cy - 17, cx, cy - 13, COLOR_WHITE);
    gfx_draw_line(cx, cy + 13, cx, cy + 17, COLOR_WHITE);
    gfx_draw_line(cx - 17, cy, cx - 13, cy, COLOR_WHITE);
    gfx_draw_line(cx + 13, cy, cx + 17, cy, COLOR_WHITE);

    gfx_draw_line(cx + 10, cy - 10, cx + 12, cy - 12, 0x0090CAF9);
    gfx_draw_line(cx - 10, cy - 10, cx - 12, cy - 12, 0x0090CAF9);
    gfx_draw_line(cx + 10, cy + 10, cx + 12, cy + 12, 0x0090CAF9);
    gfx_draw_line(cx - 10, cy + 10, cx - 12, cy + 12, 0x0090CAF9);

    for (int i = 0; i <= 14; i++) {
        int nx = cx + (i * 12) / 14;
        int ny = cy - (i * 12) / 14;
        gfx_draw_line(cx, cy, nx, ny, 0x00FF3B30);
        gfx_draw_line(cx - 2, cy - 2, nx, ny, 0x00D32F2F);
    }

    for (int i = 0; i <= 14; i++) {
        int sx = cx - (i * 12) / 14;
        int sy = cy + (i * 12) / 14;
        gfx_draw_line(cx, cy, sx, sy, COLOR_WHITE);
        gfx_draw_line(cx + 2, cy + 2, sx, sy, 0x00B0BEC5);
    }

    gfx_draw_circle(cx, cy, 3, 0x00FFD700);
    gfx_draw_pixel(cx, cy, 0x008B6508);
}

// -------------------------------------------------------------
// 4. Terminal Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_terminal_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_TERMINAL, x, y, 48)) return;

    draw_squircle_base(x, y, 48, 11, 0x002C2C2E, 0x00141416, 0x0048484A);

    gfx_draw_rounded_rect(x + 4, y + 4, 40, 40, 8, 0x000C0C0E);
    gfx_draw_rounded_rect_outline(x + 4, y + 4, 40, 40, 8, 0x001C1C1E);

    gfx_draw_rect(x + 4, y + 4, 40, 8, 0x001A1A1E);
    gfx_draw_circle(x + 9, y + 8, 2, 0x00FF5F56);
    gfx_draw_circle(x + 15, y + 8, 2, 0x00FFBD2E);
    gfx_draw_circle(x + 21, y + 8, 2, 0x0027C93F);

    int px = x + 10;
    int py = y + 18;
    gfx_draw_line(px, py, px + 8, py + 5, 0x0030D158);
    gfx_draw_line(px + 1, py, px + 9, py + 5, 0x0030D158);
    gfx_draw_line(px + 8, py + 5, px, py + 10, 0x0030D158);
    gfx_draw_line(px + 9, py + 5, px + 1, py + 10, 0x0030D158);

    gfx_draw_rect(px + 12, py + 7, 8, 3, 0x005AC8FA);

    gfx_draw_rect(px, py + 15, 26, 2, 0x003A3A3C);
    gfx_draw_rect(px, py + 19, 16, 2, 0x0030D158);
}

// -------------------------------------------------------------
// 5. Activity Monitor Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_sysmon_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_SYSMON, x, y, 48)) return;

    draw_squircle_base(x, y, 48, 11, 0x00323238, 0x001C1C1E, 0x000A84FF);

    int cx = x + 24;
    int cy = y + 24;
    gfx_draw_circle(cx, cy, 15, 0x0026262C);
    gfx_draw_circle(cx, cy, 8, 0x001C1C1E);

    gfx_draw_line(x + 6, cy, x + 42, cy, 0x002A2A34);
    gfx_draw_line(cx, y + 6, cx, y + 42, 0x002A2A34);
    gfx_draw_line(x + 6, cy - 10, x + 42, cy - 10, 0x0022222A);
    gfx_draw_line(x + 6, cy + 10, x + 42, cy + 10, 0x0022222A);

    gfx_draw_line(x + 6, cy, x + 12, cy, 0x0030D158);
    gfx_draw_line(x + 12, cy, x + 15, cy - 4, 0x0030D158);
    gfx_draw_line(x + 15, cy - 4, x + 18, cy, 0x0030D158);
    gfx_draw_line(x + 18, cy, x + 20, cy + 4, 0x0030D158);
    gfx_draw_line(x + 20, cy + 4, x + 24, cy - 15, 0x0030D158);
    gfx_draw_line(x + 24, cy - 15, x + 28, cy + 14, 0x0030D158);
    gfx_draw_line(x + 28, cy + 14, x + 31, cy - 3, 0x0030D158);
    gfx_draw_line(x + 31, cy - 3, x + 35, cy, 0x0030D158);
    gfx_draw_line(x + 35, cy, x + 42, cy, 0x0030D158);

    gfx_draw_pixel(x + 24, cy - 16, 0x00A3FFB8);
}

// -------------------------------------------------------------
// 6. Calculator Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_calculator_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_CALCULATOR, x, y, 48)) return;

    draw_squircle_base(x, y, 48, 11, 0x00FF9F0A, 0x00D95300, 0x00B84300);

    gfx_draw_rounded_rect(x + 6, y + 6, 36, 11, 3, 0x00201408);
    gfx_draw_rect(x + 26, y + 10, 12, 3, COLOR_WHITE);

    int mid_x = x + 24;
    int mid_y = y + 29;

    gfx_draw_rounded_rect(x + 6, y + 19, 16, 11, 3, 0x00E68A00);
    gfx_draw_rounded_rect(mid_x + 2, y + 19, 16, 11, 3, 0x00E68A00);
    gfx_draw_rounded_rect(x + 6, mid_y + 2, 16, 11, 3, 0x00E68A00);
    gfx_draw_rounded_rect(mid_x + 2, mid_y + 2, 16, 11, 3, 0x00C44600);

    gfx_draw_string_transparent(x + 11, y + 21, "+", COLOR_WHITE);
    gfx_draw_string_transparent(mid_x + 7, y + 21, "-", COLOR_WHITE);
    gfx_draw_string_transparent(x + 11, mid_y + 4, "*", COLOR_WHITE);
    gfx_draw_string_transparent(mid_x + 7, mid_y + 4, "=", COLOR_WHITE);
}

// -------------------------------------------------------------
// 7. Paint Studio Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_paint_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_PAINT, x, y, 48)) return;

    draw_squircle_base(x, y, 48, 11, 0x00AF52DE, 0x006B179E, 0x00520F7A);

    int cx = x + 24;
    int cy = y + 24;
    gfx_draw_circle(cx, cy, 16, 0x00F5F5F7);
    gfx_draw_circle(cx + 6, cy + 4, 12, 0x00F5F5F7);
    gfx_draw_circle(cx - 6, cy + 6, 4, 0x006B179E);

    gfx_draw_circle(x + 14, y + 16, 3, 0x00FF3B30);
    gfx_draw_circle(x + 22, y + 12, 3, 0x00FF9500);
    gfx_draw_circle(x + 30, y + 13, 3, 0x00FFCC00);
    gfx_draw_circle(x + 36, y + 20, 3, 0x0034C759);
    gfx_draw_circle(x + 36, y + 29, 3, 0x00007AFF);
    gfx_draw_circle(x + 28, y + 35, 3, 0x00AF52DE);

    gfx_draw_line(x + 8, y + 40, x + 34, y + 14, 0x008B4513);
    gfx_draw_line(x + 9, y + 40, x + 35, y + 14, 0x00A0522D);
    gfx_draw_line(x + 34, y + 14, x + 38, y + 10, 0x00D1D1D6);
    gfx_draw_line(x + 38, y + 10, x + 41, y + 7, 0x00007AFF);
}

// -------------------------------------------------------------
// 8. Notes Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_notes_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_NOTES, x, y, 48)) return;

    draw_squircle_base(x, y, 48, 11, 0x00FFE066, 0x00F5C400, 0x00D4A000);

    gfx_draw_rect(x + 3, y + 3, 42, 8, 0x00D97706);
    for (int dx = 6; dx <= 40; dx += 4) {
        gfx_draw_pixel(x + dx, y + 9, 0x00FFE082);
    }

    for (int ly = 16; ly <= 40; ly += 6) {
        gfx_draw_line(x + 6, y + ly, x + 42, y + ly, 0x00E0B800);
    }

    gfx_draw_line(x + 12, y + 11, x + 12, y + 43, 0x00FF5252);
    gfx_draw_line(x + 15, y + 19, x + 32, y + 19, 0x00786500);
    gfx_draw_line(x + 15, y + 25, x + 38, y + 25, 0x00786500);
    gfx_draw_line(x + 15, y + 31, x + 26, y + 31, 0x00786500);

    gfx_draw_line(x + 28, y + 38, x + 42, y + 24, 0x00FFB300);
    gfx_draw_line(x + 42, y + 24, x + 44, y + 22, 0x00CFD8DC);
    gfx_draw_line(x + 44, y + 22, x + 46, y + 20, 0x00FF80AB);
    gfx_draw_line(x + 28, y + 38, x + 25, y + 41, 0x00212121);
}

// -------------------------------------------------------------
// 9. Music Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_music_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_MUSIC, x, y, 48)) return;

    draw_squircle_base(x, y, 48, 11, 0x00FF2D55, 0x00C2185B, 0x00A00E44);

    gfx_draw_circle(x + 16, y + 33, 4, COLOR_WHITE);
    gfx_draw_circle(x + 30, y + 29, 4, COLOR_WHITE);

    gfx_draw_line(x + 19, y + 33, x + 19, y + 15, COLOR_WHITE);
    gfx_draw_line(x + 20, y + 33, x + 20, y + 15, COLOR_WHITE);
    gfx_draw_line(x + 33, y + 29, x + 33, y + 11, COLOR_WHITE);
    gfx_draw_line(x + 34, y + 29, x + 34, y + 11, COLOR_WHITE);

    for (int b = 0; b < 5; b++) {
        gfx_draw_line(x + 19, y + 15 + b, x + 34, y + 11 + b, COLOR_WHITE);
    }
}

// -------------------------------------------------------------
// 10. About This Mac Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_about_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_ABOUT, x, y, 48)) return;

    draw_squircle_base(x, y, 48, 11, 0x0048484A, 0x00242426, 0x00636366);
    icon_draw_apple_logo(x + 16, y + 16, COLOR_WHITE);
}

// -------------------------------------------------------------
// 11. App Store Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_appstore_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_APPSTORE, x, y, 48)) return;

    draw_squircle_base(x, y, 48, 11, 0x00007AFF, 0x00004CB8, 0x00003580);

    gfx_draw_line(x + 12, y + 36, x + 24, y + 12, COLOR_WHITE);
    gfx_draw_line(x + 13, y + 36, x + 25, y + 12, COLOR_WHITE);
    gfx_draw_line(x + 14, y + 36, x + 26, y + 12, COLOR_WHITE);

    gfx_draw_line(x + 36, y + 36, x + 24, y + 12, COLOR_WHITE);
    gfx_draw_line(x + 35, y + 36, x + 23, y + 12, COLOR_WHITE);
    gfx_draw_line(x + 34, y + 36, x + 22, y + 12, COLOR_WHITE);

    gfx_draw_rect(x + 16, y + 26, 16, 4, COLOR_WHITE);
}

// -------------------------------------------------------------
// 12. System Settings Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_settings_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_SETTINGS, x, y, 48)) return;

    draw_squircle_base(x, y, 48, 11, 0x006C6C70, 0x0048484A, 0x003A3A3C);

    int cx = x + 22, cy = y + 22;
    gfx_draw_circle(cx, cy, 11, 0x00E5E5EA);
    gfx_draw_rect(cx - 14, cy - 2, 28, 4, 0x00E5E5EA);
    gfx_draw_rect(cx - 2, cy - 14, 4, 28, 0x00E5E5EA);
    gfx_draw_circle(cx, cy, 4, 0x0048484A);

    int ax = x + 33, ay = y + 33;
    gfx_draw_circle(ax, ay, 8, 0x008E8E93);
    gfx_draw_rect(ax - 10, ay - 2, 20, 4, 0x008E8E93);
    gfx_draw_rect(ax - 2, ay - 10, 4, 20, 0x008E8E93);
    gfx_draw_circle(ax, ay, 3, 0x0048484A);
}

// -------------------------------------------------------------
// 13. Trash Dock Icon (48x48)
// -------------------------------------------------------------
void icon_draw_trash_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_TRASH, x, y, 48)) return;

    int s = 48;
    int r = 11;
    draw_squircle_base(x, y, s, r, 0x0048484A, 0x002C2C2E, 0x003A3A3C);

    gfx_draw_circle(x + 20, y + 15, 4, 0x00E5E5EA);
    gfx_draw_circle(x + 28, y + 14, 5, COLOR_WHITE);
    gfx_draw_circle(x + 24, y + 17, 3, 0x00D1D1D6);

    gfx_draw_rounded_rect(x + 10, y + 16, 28, 4, 2, COLOR_WHITE);
    gfx_draw_rounded_rect_outline(x + 10, y + 16, 28, 4, 2, 0x00AEAEB2);

    gfx_draw_line(x + 13, y + 20, x + 16, y + 40, 0x00D1D1D6);
    gfx_draw_line(x + 35, y + 20, x + 32, y + 40, 0x00D1D1D6);
    gfx_draw_line(x + 16, y + 40, x + 32, y + 40, 0x00D1D1D6);

    gfx_draw_line(x + 18, y + 20, x + 20, y + 40, 0x008E8E93);
    gfx_draw_line(x + 24, y + 20, x + 24, y + 40, 0x00AEAEB2);
    gfx_draw_line(x + 30, y + 20, x + 28, y + 40, 0x008E8E93);
}

// -------------------------------------------------------------
// Desktop Drive Icon (48x48)
// -------------------------------------------------------------
void icon_draw_drive_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_DRIVE, x, y, 48)) return;

    gfx_draw_rounded_rect(x + 2, y + 8, 44, 32, 6, 0x008E8E93);
    gfx_draw_rounded_rect_outline(x + 2, y + 8, 44, 32, 6, 0x00AEAEB2);
    gfx_draw_line(x + 6, y + 9, x + 42, y + 9, COLOR_WHITE);

    gfx_draw_rect(x + 8, y + 14, 32, 14, 0x002C2C2E);
    gfx_draw_rect_outline(x + 8, y + 14, 32, 14, 0x001C1C1E);

    gfx_draw_circle(x + 12, y + 33, 2, 0x0030D158);
    gfx_draw_pixel(x + 12, y + 33, COLOR_WHITE);

    gfx_draw_rect(x + 18, y + 32, 22, 2, 0x003A3A3C);
}

// -------------------------------------------------------------
// Desktop Folder Icon (48x48)
// -------------------------------------------------------------
void icon_draw_folder_48(int x, int y) {
    if (try_draw_theme_icon(ICON_ID_FOLDER, x, y, 48)) return;

    gfx_draw_rounded_rect(x + 6, y + 10, 16, 8, 3, 0x001976D2);
    gfx_draw_rounded_rect(x + 4, y + 14, 40, 28, 4, 0x001976D2);

    gfx_draw_rounded_rect(x + 4, y + 18, 40, 24, 4, 0x0042A5F5);
    gfx_draw_line(x + 6, y + 19, x + 42, y + 19, 0x0090CAF9);
    gfx_draw_rounded_rect_outline(x + 4, y + 18, 40, 24, 4, 0x001565C0);

    icon_draw_apple_logo(x + 16, y + 23, 0x001565C0);
}

// -------------------------------------------------------------
// Menu Bar Extras & Control Glyphs (16x16 / 24x24)
// -------------------------------------------------------------
void icon_draw_wifi(int x, int y, uint32_t color) {
    gfx_draw_line(x, y + 2, x + 4, y, color);
    gfx_draw_line(x + 4, y, x + 8, y + 2, color);

    gfx_draw_line(x + 2, y + 5, x + 4, y + 3, color);
    gfx_draw_line(x + 4, y + 3, x + 6, y + 5, color);

    gfx_draw_pixel(x + 4, y + 7, color);
    gfx_draw_pixel(x + 3, y + 7, color);
    gfx_draw_pixel(x + 5, y + 7, color);
}

void icon_draw_battery(int x, int y, uint32_t color) {
    gfx_draw_rect_outline(x, y + 2, 18, 9, color);
    gfx_draw_rect(x + 18, y + 5, 2, 3, color);
    gfx_draw_rect(x + 2, y + 4, 13, 5, 0x0030D158);
}

void icon_draw_search(int x, int y, uint32_t color) {
    gfx_draw_circle(x + 5, y + 5, 4, color);
    gfx_draw_circle(x + 5, y + 5, 3, 0x001C1C1E);

    gfx_draw_line(x + 8, y + 8, x + 13, y + 13, color);
    gfx_draw_line(x + 9, y + 8, x + 14, y + 13, color);
}

void icon_draw_control_center(int x, int y, uint32_t color) {
    gfx_draw_rounded_rect(x, y + 1, 14, 4, 2, color);
    gfx_draw_circle(x + 3, y + 3, 2, COLOR_WHITE);

    gfx_draw_rounded_rect(x, y + 7, 14, 4, 2, color);
    gfx_draw_circle(x + 11, y + 9, 2, COLOR_WHITE);
}

void icon_draw_siri(int x, int y) {
    gfx_draw_circle(x + 6, y + 6, 6, 0x00FF2D55);
    gfx_draw_circle(x + 6, y + 6, 4, 0x005AC8FA);
    gfx_draw_circle(x + 6, y + 6, 2, 0x00FFD60A);
    gfx_draw_pixel(x + 6, y + 6, COLOR_WHITE);
}

// -------------------------------------------------------------
// Scaled Icon Drawing Dispatcher
// -------------------------------------------------------------
void icon_draw_scaled(icon_id_t id, int x, int y, int size) {
    if (size <= 0) size = 48;
    if (try_draw_theme_icon(id, x, y, size)) return;

    // Vector fallback dispatcher
    switch (id) {
        case ICON_ID_FINDER:      icon_draw_finder_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_LAUNCHPAD:   icon_draw_launchpad_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_SAFARI:      icon_draw_safari_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_TERMINAL:    icon_draw_terminal_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_SYSMON:      icon_draw_sysmon_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_CALCULATOR:  icon_draw_calculator_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_PAINT:       icon_draw_paint_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_NOTES:       icon_draw_notes_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_MUSIC:       icon_draw_music_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_SETTINGS:    icon_draw_settings_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_APPSTORE:    icon_draw_appstore_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_ABOUT:       icon_draw_about_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_TRASH:       icon_draw_trash_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_DRIVE:       icon_draw_drive_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_FOLDER:      icon_draw_folder_48(x + (size - 48)/2, y + (size - 48)/2); break;
        case ICON_ID_ABOUT_64:    icon_draw_apple_64(x + (size - 64)/2, y + (size - 64)/2); break;
        default: break;
    }
}



