# RatanaOS — Global UI Scaling Engine Specification

## 1. Overview & Architecture

RatanaOS provides a centralized UI scaling engine designed for HiDPI displays (4K/Retina panels) and accessibility zoom. The scaling engine bridges **logical coordinates** (developer coordinates) with **physical framebuffer pixels** seamlessly.

```text
Logical Layout (e.g., 1024x768 @ 1.0x)
         │
         ▼  [ui_scale_val / ui_scale_rect]
Physical Framebuffer Rasterization (e.g., 2048x1536 @ 2.0x)
```

---

## 2. Supported Display Scales

The system supports 5 discrete global UI scale targets:

| UI Scale Factor | Percentage | Typical Resolution / Target Hardware |
| :---: | :---: | :--- |
| **1.0x** | 100% | 1024x768, 1280x800, standard legacy monitors |
| **1.25x** | 125% | 1920x1080 (1080p) laptops (13"-15") |
| **1.5x** | 150% | 2560x1440 (QHD) monitors |
| **1.75x** | 175% | 2880x1800 Retina MacBook panels |
| **2.0x** | 200% | 3840x2160 (4K UHD) displays |

---

## 3. Coordinate Transformation Math

### Logical to Physical (Forward Mapping)
For coordinate $(x_L, y_L)$ and scale factor $S = \frac{\text{scale\_pct}}{100}$:

$$x_P = \left\lfloor \frac{x_L \times \text{scale\_pct} + 50}{100} \right\rfloor, \quad y_P = \left\lfloor \frac{y_L \times \text{scale\_pct} + 50}{100} \right\rfloor$$

### Physical to Logical (Inverse Event Mapping)
Mouse clicks and cursor coordinates from hardware input $(x_P, y_P)$ are mapped back to logical window targets:

$$x_L = \left\lfloor \frac{x_P \times 100}{\text{scale\_pct}} \right\rfloor, \quad y_L = \left\lfloor \frac{y_P \times 100}{\text{scale\_pct}} \right\rfloor$$

---

## 4. UI Metric Scaling Table

| Metric | Base Value (1.0x) | 1.25x | 1.5x | 2.0x |
| :--- | :---: | :---: | :---: | :---: |
| **Window Titlebar Height** | 28 px | 35 px | 42 px | 56 px |
| **Traffic Light Diameter** | 12 px | 15 px | 18 px | 24 px |
| **Traffic Light Spacing** | 8 px | 10 px | 12 px | 16 px |
| **Window Corner Radius** | 10 px | 13 px | 15 px | 20 px |
| **Window Drop Shadow** | 16 px | 20 px | 24 px | 32 px |
| **Dock Item Icon Size** | 48 px | 60 px | 72 px | 96 px |
| **Menubar Height** | 24 px | 30 px | 36 px | 48 px |

---

## 5. API Reference & Usage

```c
/* Set the global scale percentage (100, 125, 150, 175, 200) */
void ui_scale_set(uint8_t scale_pct);

/* Retrieve current scale percentage */
uint8_t ui_scale_get(void);

/* Scale an integer dimension */
int ui_scale_val(int val);

/* Scale a 2D point or dimension */
void ui_scale_coord(int *x, int *y);

/* Unscale a hardware coordinate to logical space */
void ui_unscale_coord(int *x, int *y);
```

### Example Application Integration
```c
void my_app_render(window_t *win) {
    int padding = ui_scale_val(16);
    int btn_w   = ui_scale_val(100);
    int btn_h   = ui_scale_val(32);
    
    gfx_draw_rounded_rect(win->x + padding, win->y + padding, 
                          btn_w, btn_h, 
                          ui_scale_val(6), 
                          theme->accent_color);
}
```
