# RatanaOS — Dock Subsystem Specification

## 1. Overview

The RatanaOS Dock (`src/kernel/dock.c`, `src/include/dock.h`) is an application launcher, task switcher, and status dock modeled after macOS Sequoia. It supports 3 screen edge positions, 4 visual styling modes, smooth cursor magnification, and dynamic auto-hide.

---

## 2. Screen Edge Positioning

The dock can be anchored to any of three display edges:

```text
       ┌───────────────────────────────┐
       │ Menubar                       │
       ├────┬─────────────────────┬────┤
       │ L  │                     │ R  │
       │ e  │                     │ i  │
       │ f  │      Desktop        │ g  │
       │ t  │                     │ h  │
       │    │                     │ t  │
       ├────┴─────────────────────┴────┤
       │             Bottom            │
       └───────────────────────────────┘
```

1. **Bottom (Default)**: Horizontal dock centered along screen bottom.
2. **Left**: Vertical dock centered along screen left edge.
3. **Right**: Vertical dock centered along screen right edge.

When positioned vertically on Left or Right edges:
- Dock container dimensions invert ($W \leftrightarrow H$).
- Icon flow switches from horizontal $(x + i \times \text{step})$ to vertical $(y + i \times \text{step})$.
- Running indicator dots switch from underneath the icon to the outer edge of the icon.

---

## 3. Visual Styling Modes

| Style Enum | Name | Background Appearance | Border | Corner Radius |
| :--- | :--- | :--- | :--- | :---: |
| `DOCK_STYLE_GLASS` | **Frosted Glass (Default)** | High-blur translucent acrylic (`alpha=180`) | 1px subtle white highlight | 18 px |
| `DOCK_STYLE_CLASSIC` | **Classic 2D** | Opaque charcoal slate (`alpha=255`) | 1px crisp divider border | 6 px |
| `DOCK_STYLE_TRANSPARENT` | **Transparent** | Completely clear background | None | 0 px |
| `DOCK_STYLE_COMPACT` | **Compact** | Reduced padding (40px base icons), high contrast | 1px border | 12 px |

---

## 4. Magnification Engine

When magnification is enabled, icon sizes scale dynamically based on cursor proximity using a Gaussian/parabolic distance falloff:

$$S(d) = S_{\text{base}} + (S_{\text{max}} - S_{\text{base}}) \times \max\left(0, 1 - \frac{d^2}{R^2}\right)$$

- $S_{\text{base}} = 48\text{ px}$ (or $40\text{ px}$ in Compact)
- $S_{\text{max}} = 64\text{ px}$ (or $80\text{ px}$ on HiDPI)
- Proximity radius $R = 96\text{ px}$
- $d = |\text{cursor\_coord} - \text{icon\_center}|$

---

## 5. Auto-Hide Proximity Logic

When `dock_autohide` is enabled:
1. The dock remains hidden until the cursor reaches the proximity zone ($\le 40\text{ px}$ from the screen edge).
2. Proximity zone depends on position:
   - **Bottom**: `mouse_y >= screen_height - 40`
   - **Left**: `mouse_x <= 40`
   - **Right**: `mouse_x >= screen_width - 40`
3. Once revealed, the dock stays visible while the cursor remains within the dock bounds plus an 80 px tolerance pad.
4. When the cursor leaves the boundary, the dock animates/slides offscreen.

---

## 6. Running Application Indicators

Applications with active processes display an indicator dot:
- **Bottom Dock**: 4 px rounded dot centered at $(x_{\text{center}}, y_{\text{dock}} + h - 5)$.
- **Left Dock**: 4 px rounded dot centered at $(x_{\text{dock}} + 5, y_{\text{center}})$.
- **Right Dock**: 4 px rounded dot centered at $(x_{\text{dock}} + w - 5, y_{\text{center}})$.
- Dot color: High-contrast white (`0xFFFFFFFF`) with soft anti-aliased edge.
