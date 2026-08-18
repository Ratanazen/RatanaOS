#include "../include/gfx.h"
#include "../include/heap.h"
#include "../include/string.h"
#include "../include/font8x16.h"

static gfx_context_t ctx = {0, 0, 0, 0, 0, 0, false};

bool gfx_init(multiboot_info_t* mbi) {
    if (!mbi || !(mbi->flags & (1 << 12))) {
        // Framebuffer flag not set by bootloader, fallback
        ctx.width = 1024;
        ctx.height = 768;
        ctx.pitch = 1024 * 4;
        ctx.bpp = 32;
        ctx.frontbuffer = (uint32_t*)0xFD000000; // Standard QEMU default VBE FB address
        ctx.backbuffer = (uint32_t*)kmalloc(ctx.width * ctx.height * sizeof(uint32_t));
        ctx.active = (ctx.backbuffer != NULL);
        return ctx.active;
    }

    ctx.frontbuffer = (uint32_t*)((uintptr_t)mbi->framebuffer_addr);
    ctx.width = mbi->framebuffer_width;
    ctx.height = mbi->framebuffer_height;
    ctx.pitch = mbi->framebuffer_pitch;
    ctx.bpp = mbi->framebuffer_bpp;

    size_t fb_size = ctx.width * ctx.height * sizeof(uint32_t);
    ctx.backbuffer = (uint32_t*)kmalloc(fb_size);

    if (!ctx.backbuffer) {
        ctx.backbuffer = ctx.frontbuffer; // Fallback to direct rendering if heap allocation fails
    }

    ctx.active = true;
    gfx_clear(COLOR_BG_DARK);
    gfx_swap_buffers();
    return true;
}

bool gfx_is_active(void) {
    return ctx.active;
}

int gfx_get_width(void) {
    return (int)ctx.width;
}

int gfx_get_height(void) {
    return (int)ctx.height;
}

void gfx_clear(uint32_t color) {
    if (!ctx.active || !ctx.backbuffer) return;
    size_t count = ctx.width * ctx.height;
    for (size_t i = 0; i < count; i++) {
        ctx.backbuffer[i] = color;
    }
}

void gfx_draw_pixel(int x, int y, uint32_t color) {
    if (!ctx.active || !ctx.backbuffer) return;
    if (x < 0 || x >= (int)ctx.width || y < 0 || y >= (int)ctx.height) return;
    ctx.backbuffer[y * ctx.width + x] = color;
}

void gfx_draw_rect(int x, int y, int w, int h, uint32_t color) {
    if (!ctx.active || !ctx.backbuffer) return;
    if (x >= (int)ctx.width || y >= (int)ctx.height) return;

    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = (x + w > (int)ctx.width) ? (int)ctx.width : x + w;
    int y1 = (y + h > (int)ctx.height) ? (int)ctx.height : y + h;

    for (int cy = y0; cy < y1; cy++) {
        uint32_t* row = &ctx.backbuffer[cy * ctx.width];
        for (int cx = x0; cx < x1; cx++) {
            row[cx] = color;
        }
    }
}

void gfx_draw_rect_outline(int x, int y, int w, int h, uint32_t color) {
    if (w <= 0 || h <= 0) return;
    gfx_draw_rect(x, y, w, 1, color);             // Top
    gfx_draw_rect(x, y + h - 1, w, 1, color);     // Bottom
    gfx_draw_rect(x, y, 1, h, color);             // Left
    gfx_draw_rect(x + w - 1, y, 1, h, color);     // Right
}

void gfx_draw_circle(int cx, int cy, int radius, uint32_t color) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        gfx_draw_pixel(cx + x, cy + y, color);
        gfx_draw_pixel(cx + y, cy + x, color);
        gfx_draw_pixel(cx - y, cy + x, color);
        gfx_draw_pixel(cx - x, cy + y, color);
        gfx_draw_pixel(cx - x, cy - y, color);
        gfx_draw_pixel(cx - y, cy - x, color);
        gfx_draw_pixel(cx + y, cy - x, color);
        gfx_draw_pixel(cx + x, cy - y, color);

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void gfx_draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = (x1 >= x0) ? (x1 - x0) : (x0 - x1);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = (y1 >= y0) ? -(y1 - y0) : -(y0 - y1);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (1) {
        gfx_draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void gfx_draw_gradient_v(int x, int y, int w, int h, uint32_t c1, uint32_t c2) {
    if (h <= 0) return;
    uint8_t r1 = (c1 >> 16) & 0xFF, g1 = (c1 >> 8) & 0xFF, b1 = c1 & 0xFF;
    uint8_t r2 = (c2 >> 16) & 0xFF, g2 = (c2 >> 8) & 0xFF, b2 = c2 & 0xFF;

    for (int i = 0; i < h; i++) {
        uint8_t r = r1 + ((r2 - r1) * i) / h;
        uint8_t g = g1 + ((g2 - g1) * i) / h;
        uint8_t b = b1 + ((b2 - b1) * i) / h;
        uint32_t color = COLOR_RGB(r, g, b);
        gfx_draw_rect(x, y + i, w, 1, color);
    }
}

void gfx_draw_char(int x, int y, char c, uint32_t fg, uint32_t bg) {
    uint8_t idx = (uint8_t)c;
    if (idx >= 128) idx = '?';

    const uint8_t* glyph = font8x16_basic[idx];
    for (int row = 0; row < 16; row++) {
        uint8_t line = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (line & (0x80 >> col)) {
                gfx_draw_pixel(x + col, y + row, fg);
            } else if (bg != 0xFF000000) { // Transparent background flag
                gfx_draw_pixel(x + col, y + row, bg);
            }
        }
    }
}

void gfx_draw_string(int x, int y, const char* str, uint32_t fg, uint32_t bg) {
    if (!str) return;
    int cur_x = x;
    int cur_y = y;

    while (*str) {
        if (*str == '\n') {
            cur_x = x;
            cur_y += 18;
        } else {
            gfx_draw_char(cur_x, cur_y, *str, fg, bg);
            cur_x += 8;
        }
        str++;
    }
}

void gfx_draw_string_transparent(int x, int y, const char* str, uint32_t fg) {
    gfx_draw_string(x, y, str, fg, 0xFF000000);
}

void gfx_swap_buffers(void) {
    if (!ctx.active || !ctx.frontbuffer || !ctx.backbuffer || ctx.frontbuffer == ctx.backbuffer) return;
    memcpy(ctx.frontbuffer, ctx.backbuffer, ctx.width * ctx.height * sizeof(uint32_t));
}
