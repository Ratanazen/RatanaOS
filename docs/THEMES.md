# RatanaOS — macOS Theme Engine Specification & Guide

## 1. Architectural Overview

The RatanaOS Theme Engine (`src/kernel/theme.c`, `src/include/theme.h`) centralizes all desktop, window, widget, and panel styling into an atomic, coherent semantic design system. It preserves the clean macOS Sequoia aesthetic while allowing comprehensive user customization at runtime without kernel recompilation.

```text
               ┌────────────────────────────────────────────────────────┐
               │              Global Settings Manager                   │
               │             (/etc/ratana/settings.conf)                │
               └─────────────────────────┬──────────────────────────────┘
                                         │ loads/saves
                                         ▼
               ┌────────────────────────────────────────────────────────┐
               │             Central Theme Engine (ui_theme_t)          │
               │  - Mode: Light / Dark / Auto (RTC CMOS Time)           │
               │  - Accent: 8 macOS Sequoia Color Swatches              │
               │  - Window Style: macOS, Classic, Minimal, Transparent  │
               │  - Dock Style: Glass, Classic, Transparent, Compact    │
               │  - Dock Position: Bottom, Left, Right                  │
               │  - Menu Bar Style: Glass, Solid, Transparent           │
               │  - Font Scaling: 80% - 150%                            │
               └──────────┬──────────────────────────────┬──────────────┘
                          │                              │
          ┌───────────────┴───────────────┐              │
          ▼                               ▼              ▼
┌──────────────────┐            ┌──────────────────┐  ┌──────────────────┐
│  Desktop Menubar │            │  Dock Subsystem  │  │  Window Manager  │
│  (menubar.c)     │            │  (dock.c)        │  │  (gui.c, gfx.c)  │
└──────────────────┘            └──────────────────┘  └──────────────────┘
```

---

## 2. Built-in Theme Presets

RatanaOS provides five operating presets out of the box:

| Preset ID | Name | Mode | Description |
| :--- | :--- | :--- | :--- |
| `THEME_RATANA_DARK` | RatanaOS Dark | Dark | Default Sequoia Dark Theme: deep zinc/obsidian surfaces, crisp borders, blue accent. |
| `THEME_RATANA_LIGHT` | RatanaOS Light | Light | Default Sequoia Light Theme: bright silver/pearl surfaces, subtle borders, blue accent. |
| `THEME_MACOS_DARK` | macOS Dark | Dark | macOS Tahoe/Dark Slate: dark charcoal glass surfaces, low-contrast subtle dividers. |
| `THEME_MACOS_LIGHT` | macOS Light | Light | macOS Tahoe/Light Aluminum: frosted white glass panels, subtle dark drop shadows. |
| `THEME_MODE_AUTO` | Auto (RTC Dynamic) | Dynamic | Automatically evaluates CMOS RTC clock (07:00–18:59 = Light; 19:00–06:59 = Dark). |

---

## 3. Semantic Visual Tokens

All rendering routines reference tokens in `ui_theme_t` rather than hardcoded 32-bit ARGB values.

### Color & Geometry Tokens

```c
typedef struct {
    /* Base Color Tokens */
    uint32_t bg_primary;          /* Primary desktop/canvas background */
    uint32_t bg_secondary;        /* Secondary background (panels, sidebars) */
    uint32_t surface;             /* Elevated surfaces (cards, popovers, sheets) */
    uint32_t surface_secondary;   /* Grouped surface items, list item backgrounds */
    
    /* Content & Typography Tokens */
    uint32_t text_primary;        /* Primary typography (high contrast) */
    uint32_t text_secondary;      /* Secondary typography (labels, subtitles) */
    uint32_t text_muted;          /* Disabled/placeholder text */
    
    /* Interactive & Accent Tokens */
    uint32_t accent_color;        /* Active accent fill */
    uint32_t accent_hover;        /* Accent under mouse hover */
    uint32_t accent_pressed;      /* Accent under active click */
    
    /* Structural Tokens */
    uint32_t border;              /* Window and control dividers */
    uint32_t border_focus;        /* Focused ring border */
    uint32_t selection;           /* Highlight/selection fill */
    
    /* Component Backgrounds */
    uint32_t menubar_bg;          /* Top menu bar base fill */
    uint32_t dock_bg;             /* Bottom/side dock container fill */
    uint32_t shadow;              /* Drop shadow color */
    
    /* Metrics & Stylistic Tokens */
    uint8_t  alpha_glass;         /* Translucency alpha (0-255) */
    uint8_t  corner_radius;       /* Window frame corner radius (px) */
    uint8_t  shadow_radius;       /* Window drop shadow blur/spread */
    uint8_t  window_border_width; /* Window border thickness in px */
    uint8_t  font_scale;          /* Font scaling factor (80 to 150) */
    
    /* Style Enumerations */
    window_style_t  window_style;
    dock_position_t dock_position;
    dock_style_t    dock_style;
    menubar_style_t menubar_style;
    bool            dock_autohide;
    theme_mode_t    mode;
} ui_theme_t;
```

---

## 4. Accent Color Palette

Users can personalize the desktop accent across 8 macOS Sequoia swatches via System Settings or `theme_set_accent()`:

| Accent Name | Primary (Hex) | Hover (Hex) | Pressed (Hex) | Suggested Context |
| :--- | :--- | :--- | :--- | :--- |
| **Blue (Default)** | `0x0A84FF` | `0x409CFF` | `0x0062CC` | System default, professional |
| **Purple** | `0xBF5AF2` | `0xCF7BFF` | `0x9E3ACB` | Creative suites, media |
| **Pink** | `0xFF375F` | `0xFF6482` | `0xD91E44` | Vibrant accents, alerts |
| **Red** | `0xFF453A` | `0xFF6961` | `0xD72C22` | High-priority controls |
| **Orange** | `0xFF9F0A` | `0xFFB340` | `0xDB7D00` | Warm highlights |
| **Yellow** | `0xFFD60A` | `0xFFE040` | `0xD4AF00` | Caution, bright accents |
| **Green** | `0x30D158` | `0x5CE07C` | `0x22A842` | Success states, productivity |
| **Graphite** | `0x8E8E93` | `0xAEAEB2` | `0x636366` | Minimalist monochrome |

---

## 5. API Reference

```c
/* Initialize theme subsystem and load saved config */
void theme_init(void);

/* Get active theme instance */
ui_theme_t* theme_get_current(void);
ui_theme_t* ui_theme_get(void);

/* Apply a theme preset (updates mode, tokens, and notifies subscribers) */
void theme_apply(theme_type_t theme);
void ui_theme_set(theme_type_t theme);

/* Set light / dark / auto mode */
void theme_set_mode(theme_mode_t mode);
theme_mode_t theme_get_mode(void);

/* Change accent color */
void theme_set_accent(uint32_t color);
uint32_t theme_get_accent(void);

/* Configure window decoration style */
void theme_set_window_style(window_style_t style);

/* Configure dock position and style */
void theme_set_dock_position(dock_position_t pos);
void theme_set_dock_style(dock_style_t style);
void theme_set_dock_autohide(bool autohide);

/* Configure menu bar appearance */
void theme_set_menubar_style(menubar_style_t style);

/* Configure font scaling factor (80 to 150%) */
void theme_set_font_scale(uint8_t scale_pct);
```

---

## 6. How to Add a New Theme Preset

1. Add the enum identifier in `src/include/theme.h`:
   ```c
   typedef enum {
       THEME_RATANA_DARK = 0,
       THEME_RATANA_LIGHT,
       THEME_MACOS_DARK,
       THEME_MACOS_LIGHT,
       THEME_CUSTOM_PRESET,
       THEME_COUNT
   } theme_type_t;
   ```

2. Define its semantic token initialization table in `src/kernel/theme.c`:
   ```c
   static void theme_load_custom(ui_theme_t *t) {
       t->bg_primary = 0x1E1E2E;
       t->surface    = 0x313244;
       t->text_primary = 0xCDD6F4;
       t->accent_color = 0xCBA6F7;
       /* populate remaining tokens */
   }
   ```

3. Recompile with `make` and test in QEMU.
