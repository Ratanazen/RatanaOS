#include "../include/gfx.h"
#include "../include/font8x16.h"
#include "../include/heap.h"
#include "../include/string.h"
#include "../include/io.h"
#include "../include/pci.h"

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF

#define VBE_DISPI_INDEX_ID          0
#define VBE_DISPI_INDEX_XRES        1
#define VBE_DISPI_INDEX_YRES        2
#define VBE_DISPI_INDEX_BPP         3
#define VBE_DISPI_INDEX_ENABLE      4

#define VBE_DISPI_DISABLED          0x00
#define VBE_DISPI_ENABLED           0x01
#define VBE_DISPI_LFB_ENABLED       0x40

static gfx_context_t ctx;

static inline void bga_write(uint16_t index, uint16_t data) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, data);
}

static uint64_t find_vga_pci_bar0(void) {
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {
            uint32_t id = pci_read_config_dword(bus, slot, 0, 0x00);
            if ((id & 0xFFFF) != 0xFFFF) {
                uint32_t class_reg = pci_read_config_dword(bus, slot, 0, 0x08);
                uint8_t class_code = (class_reg >> 24) & 0xFF;
                if (class_code == 0x03) { // Display controller
                    uint32_t bar0 = pci_read_config_dword(bus, slot, 0, 0x10);
                    return (uint64_t)(bar0 & 0xFFFFFFF0);
                }
            }
        }
    }
    return 0xFD000000; // QEMU standard default
}

bool gfx_init(multiboot_info_t* mbi) {
    if (mbi && (mbi->flags & (1 << 12)) && mbi->framebuffer_addr != 0) {
        ctx.width = mbi->framebuffer_width;
        ctx.height = mbi->framebuffer_height;
        ctx.pitch = mbi->framebuffer_pitch;
        ctx.bpp = mbi->framebuffer_bpp;
        ctx.frontbuffer = (uint32_t*)(uintptr_t)mbi->framebuffer_addr;
    } else {
        // Initialize BGA 1024x768x32
        bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
        bga_write(VBE_DISPI_INDEX_XRES, 1024);
        bga_write(VBE_DISPI_INDEX_YRES, 768);
        bga_write(VBE_DISPI_INDEX_BPP, 32);
        bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

        ctx.width = 1024;
        ctx.height = 768;
        ctx.pitch = 1024 * 4;
        ctx.bpp = 32;
        ctx.frontbuffer = (uint32_t*)(uintptr_t)find_vga_pci_bar0();
    }

    size_t fb_size = (size_t)ctx.width * ctx.height * 4;
    ctx.backbuffer = (uint32_t*)kmalloc(fb_size);
    if (!ctx.backbuffer) {
        ctx.backbuffer = ctx.frontbuffer;
    } else {
        memset(ctx.backbuffer, 0, fb_size);
    }

    // Precomputed wallpaper buffer for 60 FPS rendering
    ctx.wallpaper_buffer = (uint32_t*)kmalloc(fb_size);
    gfx_init_wallpaper();

    ctx.active = true;
    return true;
}

void gfx_init_wallpaper(void) {
    if (!ctx.wallpaper_buffer) return;

    // macOS Sequoia Multi-tone Gradient: #1C1C2E -> #2E2E48 -> #141420
    uint32_t c_top = 0x001C1C2E;
    uint32_t c_mid = 0x002E2E48;
    uint32_t c_bot = 0x00141420;

    int mid_y = (int)ctx.height / 2;

    uint8_t r1 = (c_top >> 16) & 0xFF, g1 = (c_top >> 8) & 0xFF, b1 = c_top & 0xFF;
    uint8_t r2 = (c_mid >> 16) & 0xFF, g2 = (c_mid >> 8) & 0xFF, b2 = c_mid & 0xFF;
    uint8_t r3 = (c_bot >> 16) & 0xFF, g3 = (c_bot >> 8) & 0xFF, b3 = c_bot & 0xFF;

    for (int y = 0; y < (int)ctx.height; y++) {
        uint32_t col;
        if (y < mid_y) {
            uint8_t r = (uint8_t)(r1 + ((r2 - r1) * y) / (mid_y > 0 ? mid_y : 1));
            uint8_t g = (uint8_t)(g1 + ((g2 - g1) * y) / (mid_y > 0 ? mid_y : 1));
            uint8_t b = (uint8_t)(b1 + ((b2 - b1) * y) / (mid_y > 0 ? mid_y : 1));
            col = (r << 16) | (g << 8) | b;
        } else {
            int dy = y - mid_y;
            int h2 = (int)ctx.height - mid_y;
            uint8_t r = (uint8_t)(r2 + ((r3 - r2) * dy) / (h2 > 0 ? h2 : 1));
            uint8_t g = (uint8_t)(g2 + ((g3 - g2) * dy) / (h2 > 0 ? h2 : 1));
            uint8_t b = (uint8_t)(b2 + ((b3 - b2) * dy) / (h2 > 0 ? h2 : 1));
            col = (r << 16) | (g << 8) | b;
        }

        for (int x = 0; x < (int)ctx.width; x++) {
            ctx.wallpaper_buffer[y * ctx.width + x] = col;
        }
    }
}

void gfx_draw_wallpaper(void) {
    if (ctx.wallpaper_buffer && ctx.backbuffer) {
        size_t total_pixels = (size_t)ctx.width * ctx.height;
        memcpy(ctx.backbuffer, ctx.wallpaper_buffer, total_pixels * 4);
    } else {
        gfx_clear(0x00141420);
    }
}

bool gfx_is_active(void) { return ctx.active; }
int gfx_get_width(void) { return (int)ctx.width; }
int gfx_get_height(void) { return (int)ctx.height; }
uint32_t* gfx_get_backbuffer(void) { return ctx.backbuffer; }

void gfx_clear(uint32_t color) {
    if (!ctx.backbuffer) return;
    size_t total_pixels = (size_t)ctx.width * ctx.height;
    for (size_t i = 0; i < total_pixels; i++) {
        ctx.backbuffer[i] = color;
    }
}

void gfx_draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || (uint32_t)x >= ctx.width || y < 0 || (uint32_t)y >= ctx.height || !ctx.backbuffer) return;
    ctx.backbuffer[y * ctx.width + x] = color;
}

void gfx_draw_pixel_alpha(int x, int y, uint32_t color, uint8_t alpha) {
    if (x < 0 || (uint32_t)x >= ctx.width || y < 0 || (uint32_t)y >= ctx.height || !ctx.backbuffer) return;
    if (alpha == 255) {
        ctx.backbuffer[y * ctx.width + x] = color;
        return;
    }
    if (alpha == 0) return;

    uint32_t dst = ctx.backbuffer[y * ctx.width + x];
    uint8_t sr = (color >> 16) & 0xFF, sg = (color >> 8) & 0xFF, sb = color & 0xFF;
    uint8_t dr = (dst >> 16) & 0xFF, dg = (dst >> 8) & 0xFF, db = dst & 0xFF;

    uint8_t r = (uint8_t)((sr * alpha + dr * (255 - alpha)) / 255);
    uint8_t g = (uint8_t)((sg * alpha + dg * (255 - alpha)) / 255);
    uint8_t b = (uint8_t)((sb * alpha + db * (255 - alpha)) / 255);

    ctx.backbuffer[y * ctx.width + x] = (r << 16) | (g << 8) | b;
}

uint32_t gfx_get_pixel(int x, int y) {
    if (x < 0 || (uint32_t)x >= ctx.width || y < 0 || (uint32_t)y >= ctx.height || !ctx.backbuffer) return 0;
    return ctx.backbuffer[y * ctx.width + x];
}

void gfx_draw_rect(int x, int y, int w, int h, uint32_t color) {
    if (x >= (int)ctx.width || y >= (int)ctx.height || x + w <= 0 || y + h <= 0) return;

    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = (x + w > (int)ctx.width) ? (int)ctx.width : x + w;
    int y1 = (y + h > (int)ctx.height) ? (int)ctx.height : y + h;

    for (int cy = y0; cy < y1; cy++) {
        uint32_t* row = &ctx.backbuffer[cy * ctx.width + x0];
        int count = x1 - x0;
        for (int cx = 0; cx < count; cx++) {
            row[cx] = color;
        }
    }
}

void gfx_draw_rect_alpha(int x, int y, int w, int h, uint32_t color, uint8_t alpha) {
    if (x >= (int)ctx.width || y >= (int)ctx.height || x + w <= 0 || y + h <= 0) return;

    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = (x + w > (int)ctx.width) ? (int)ctx.width : x + w;
    int y1 = (y + h > (int)ctx.height) ? (int)ctx.height : y + h;

    for (int cy = y0; cy < y1; cy++) {
        for (int cx = x0; cx < x1; cx++) {
            gfx_draw_pixel_alpha(cx, cy, color, alpha);
        }
    }
}

void gfx_draw_rect_outline(int x, int y, int w, int h, uint32_t color) {
    gfx_draw_line(x, y, x + w - 1, y, color);
    gfx_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
    gfx_draw_line(x, y, x, y + h - 1, color);
    gfx_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void gfx_draw_rounded_rect(int x, int y, int w, int h, int r, uint32_t color) {
    if (r <= 0) {
        gfx_draw_rect(x, y, w, h, color);
        return;
    }
    // Main inner crosses
    gfx_draw_rect(x + r, y, w - 2 * r, h, color);
    gfx_draw_rect(x, y + r, r, h - 2 * r, color);
    gfx_draw_rect(x + w - r, y + r, r, h - 2 * r, color);

    // 4 Corner circles
    gfx_draw_circle(x + r, y + r, r, color);
    gfx_draw_circle(x + w - r - 1, y + r, r, color);
    gfx_draw_circle(x + r, y + h - r - 1, r, color);
    gfx_draw_circle(x + w - r - 1, y + h - r - 1, r, color);
}

void gfx_draw_rounded_rect_alpha(int x, int y, int w, int h, int r, uint32_t color, uint8_t alpha) {
    for (int cy = y; cy < y + h; cy++) {
        for (int cx = x; cx < x + w; cx++) {
            // Check if inside corner cutouts
            bool inside = true;
            if (cx < x + r && cy < y + r) { // Top-left
                int dx = cx - (x + r), dy = cy - (y + r);
                if (dx * dx + dy * dy > r * r) inside = false;
            } else if (cx >= x + w - r && cy < y + r) { // Top-right
                int dx = cx - (x + w - r - 1), dy = cy - (y + r);
                if (dx * dx + dy * dy > r * r) inside = false;
            } else if (cx < x + r && cy >= y + h - r) { // Bottom-left
                int dx = cx - (x + r), dy = cy - (y + h - r - 1);
                if (dx * dx + dy * dy > r * r) inside = false;
            } else if (cx >= x + w - r && cy >= y + h - r) { // Bottom-right
                int dx = cx - (x + w - r - 1), dy = cy - (y + h - r - 1);
                if (dx * dx + dy * dy > r * r) inside = false;
            }

            if (inside) {
                gfx_draw_pixel_alpha(cx, cy, color, alpha);
            }
        }
    }
}

void gfx_draw_rounded_rect_outline(int x, int y, int w, int h, int r, uint32_t color) {
    gfx_draw_line(x + r, y, x + w - r - 1, y, color);
    gfx_draw_line(x + r, y + h - 1, x + w - r - 1, y + h - 1, color);
    gfx_draw_line(x, y + r, x, y + h - r - 1, color);
    gfx_draw_line(x + w - 1, y + r, x + w - 1, y + h - r - 1, color);
}

void gfx_draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = (x1 >= x0) ? (x1 - x0) : (x0 - x1);
    int dy = (y1 >= y0) ? (y1 - y0) : (y0 - y1);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        gfx_draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void gfx_draw_circle(int xc, int yc, int r, uint32_t color) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                gfx_draw_pixel(xc + x, yc + y, color);
            }
        }
    }
}

void gfx_draw_gradient_v(int x, int y, int w, int h, uint32_t top_color, uint32_t bottom_color) {
    uint8_t r1 = (top_color >> 16) & 0xFF, g1 = (top_color >> 8) & 0xFF, b1 = top_color & 0xFF;
    uint8_t r2 = (bottom_color >> 16) & 0xFF, g2 = (bottom_color >> 8) & 0xFF, b2 = bottom_color & 0xFF;

    for (int cy = 0; cy < h; cy++) {
        uint8_t r = (uint8_t)(r1 + ((r2 - r1) * cy) / (h > 1 ? (h - 1) : 1));
        uint8_t g = (uint8_t)(g1 + ((g2 - g1) * cy) / (h > 1 ? (h - 1) : 1));
        uint8_t b = (uint8_t)(b1 + ((b2 - b1) * cy) / (h > 1 ? (h - 1) : 1));
        uint32_t col = (r << 16) | (g << 8) | b;
        gfx_draw_rect(x, y + cy, w, 1, col);
    }
}

void gfx_draw_char(int x, int y, char c, uint32_t fg, uint32_t bg) {
    if ((unsigned char)c >= 128) c = '?';
    const uint8_t* glyph = font8x16_basic[(unsigned char)c];

    for (int cy = 0; cy < 16; cy++) {
        uint8_t row = glyph[cy];
        for (int cx = 0; cx < 8; cx++) {
            if (row & (0x80 >> cx)) {
                gfx_draw_pixel(x + cx, y + cy, fg);
            } else if (bg != 0) {
                gfx_draw_pixel(x + cx, y + cy, bg);
            }
        }
    }
}

void gfx_draw_string(int x, int y, const char* str, uint32_t fg, uint32_t bg) {
    if (!str) return;
    int cur_x = x;
    while (*str) {
        if (*str == '\n') {
            cur_x = x;
            y += 16;
        } else {
            gfx_draw_char(cur_x, y, *str, fg, bg);
            cur_x += 8;
        }
        str++;
    }
}

void gfx_draw_string_transparent(int x, int y, const char* str, uint32_t fg) {
    gfx_draw_string(x, y, str, fg, 0);
}

void gfx_swap_buffers(void) {
    if (!ctx.backbuffer || !ctx.frontbuffer || ctx.backbuffer == ctx.frontbuffer) {
        return;
    }
    size_t total_pixels = (size_t)ctx.width * ctx.height;
    memcpy(ctx.frontbuffer, ctx.backbuffer, total_pixels * 4);
}
