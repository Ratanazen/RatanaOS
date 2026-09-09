# RatanaOS — Typography and Font System Specification

## 1. Overview

RatanaOS implements an Apple-inspired typography engine (`src/kernel/font.c`, `src/include/font.h`) designed for high legibility across varying display densities. The system defines semantic typographical roles, multi-factor scaling (80% to 150%), and sub-pixel glyph alignment.

---

## 2. Semantic Typographical Roles

Instead of assigning arbitrary pixel heights, applications request sizes based on semantic roles:

| Role ID | Role Name | Base Size | Line Height | Typical Usage |
| :--- | :--- | :--- | :--- | :--- |
| `FONT_ROLE_CAPTION` | Caption | 9 pt / px | 12 px | Timestamps, micro-badges, footnote text |
| `FONT_ROLE_SMALL` | Small | 11 pt / px | 14 px | Secondary labels, status bar indicators |
| `FONT_ROLE_BODY` | Body (Standard) | 13 pt / px | 16 px | Default UI controls, list items, body copy |
| `FONT_ROLE_BODY_LARGE`| Body Large | 15 pt / px | 18 px | Highlighted content, primary callout items |
| `FONT_ROLE_TITLE` | Title | 17 pt / px | 22 px | Window titlebars, section cards |
| `FONT_ROLE_HEADING` | Heading | 20 pt / px | 26 px | Application header titles, sheet headings |
| `FONT_ROLE_DISPLAY` | Display | 24 pt / px | 30 px | About dialog header, lock screen clock |

---

## 3. Dynamic Scaling Formula

To ensure crisp rendering and prevent integer overflow or underflow artifacts, typography scales dynamically using the following integer arithmetic formula:

$$\text{Scaled Size} = \left\lfloor \frac{\text{Base Size} \times \text{Scale Percentage} + 50}{100} \right\rfloor$$

```c
uint32_t font_scale_val(uint32_t base_val) {
    uint8_t scale = font_get_scale(); // e.g., 80, 100, 125, 150
    return (base_val * scale + 50) / 100;
}
```

### Reference Table Across Supported Scales

| Base Size | 80% (Compact) | 100% (Standard) | 125% (Comfortable) | 150% (Large) |
| :---: | :---: | :---: | :---: | :---: |
| **9 px** | 7 px | 9 px | 11 px | 14 px |
| **11 px** | 9 px | 11 px | 14 px | 17 px |
| **13 px** | 10 px | 13 px | 16 px | 20 px |
| **15 px** | 12 px | 15 px | 19 px | 23 px |
| **17 px** | 14 px | 17 px | 21 px | 26 px |
| **20 px** | 16 px | 20 px | 25 px | 30 px |
| **24 px** | 19 px | 24 px | 30 px | 36 px |

---

## 4. API Reference

```c
/* Initialize typography subsystem */
void font_init(void);

/* Query base point size for a semantic role */
uint32_t font_get_size_for_role(font_role_t role);

/* Apply global font scale percentage (80 - 150) */
void font_set_scale(uint8_t scale_pct);
uint8_t font_get_scale(void);

/* Scale an arbitrary dimension or coordinate by the active font scale */
uint32_t font_scale_val(uint32_t base_val);

/* Glyph and string rendering */
void font_draw_char(int x, int y, char c, uint32_t color);
void font_draw_string(int x, int y, const char *str, uint32_t color);
void font_draw_string_role(int x, int y, const char *str, font_role_t role, uint32_t color);
```

---

## 5. Application Developer Best Practices

1. **Avoid Hardcoded Glyph Offsets**: Always compute element heights and padding dynamically:
   ```c
   int text_height = font_scale_val(16);
   int item_height = text_height + font_scale_val(8);
   ```
2. **Respect Role Semantics**: Use `FONT_ROLE_TITLE` for modal and window titles to automatically track user accessibility preferences.
