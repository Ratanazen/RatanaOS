# RatanaOS — Window Decoration and Styling Specification

## 1. Overview

The RatanaOS Window Manager supports configurable window decoration styles, edge curvature, drop shadow depth, and surface opacity. Users can choose between authentic macOS Sequoia rounded glass, classic boxed frames, borderless minimal windows, and translucent acrylic panes.

---

## 2. Window Styles

```text
┌──────────────────────────┐    ┌──────────────────────────┐
│ ● ● ●       macOS Window │    │ [-] [o] [x]  Classic Win │
├──────────────────────────┤    ├──────────────────────────┤
│ - Corner Radius: 10px    │    │ - Corner Radius: 4px     │
│ - Drop Shadow: 16px      │    │ - Drop Shadow: 8px       │
│ - Traffic Light Controls │    │ - Boxed Controls         │
└──────────────────────────┘    └──────────────────────────┘

┌──────────────────────────┐    ┌──────────────────────────┐
│             Minimal Win  │    │ ● ● ●    Translucent Win │
├──────────────────────────┤    ├──────────────────────────┤
│ - Corner Radius: 0px     │    │ - Corner Radius: 12px    │
│ - Drop Shadow: None      │    │ - Drop Shadow: 20px      │
│ - Flat 1px Border        │    │ - Acrylic Alpha: 200/255 │
└──────────────────────────┘    └──────────────────────────┘
```

| Window Style Enum | Style Name | Corner Radius | Shadow Size | Alpha Opacity | Control Style |
| :--- | :--- | :---: | :---: | :---: | :--- |
| `WINDOW_STYLE_MACOS` | **macOS Sequoia** | 10 px | 16 px | 255 (Opaque) | Rounded Traffic Lights (Red, Yellow, Green) |
| `WINDOW_STYLE_CLASSIC` | **Classic** | 4 px | 8 px | 255 (Opaque) | Rectangular titlebar buttons with borders |
| `WINDOW_STYLE_MINIMAL` | **Minimalist** | 0 px | 0 px (None) | 255 (Opaque) | Flat borderless/thin border design |
| `WINDOW_STYLE_TRANSPARENT` | **Acrylic Glass** | 12 px | 20 px | 200 (Translucent)| Translucent background with blurred drop shadow |

---

## 3. Window Decoration Components

### Traffic Light Buttons
Located at $(x + 12, y + 8)$ with diameter 12 px and 8 px spacing:
- **Close Button (Red)**: `0xFF5F56` (Hover: `0xFF3B30`)
- **Minimize Button (Yellow)**: `0xFFBD2E` (Hover: `0xFF9500`)
- **Zoom/Maximize Button (Green)**: `0x27C93F` (Hover: `0x34C759`)

### Window Drop Shadow Engine
Shadows are rendered using multi-pass concentric alpha expansion:
```c
void window_draw_shadow(int x, int y, int w, int h, int radius, uint32_t color) {
    if (radius <= 0) return; // Minimal style disables shadow
    for (int r = radius; r > 0; r -= 2) {
        uint8_t alpha = (uint8_t)((radius - r + 1) * 35 / radius);
        gfx_draw_rounded_rect_outline(x - r, y - r + 4, 
                                      w + 2 * r, h + 2 * r, 
                                      12 + r, 
                                      color | ((uint32_t)alpha << 24));
    }
}
```

---

## 4. API Reference

```c
/* Configure window decoration style */
void theme_set_window_style(window_style_t style);

/* Query active window style */
window_style_t theme_get_window_style(void);

/* Render window decorations (called by Window Manager during compositing) */
void window_render_decorations(window_t *win, const ui_theme_t *theme);
```
