# RatanaOS macOS Theme Engine & UI Customization Architecture Audit Report

## 1. Existing Rendering Architecture
- **Pipeline**: Double-buffered 32-bit ARGB (TrueColor 8888) rasterizer implemented in `src/drivers/gfx.c` and declared in `src/include/gfx.h`.
- **Buffers**:
  - `ctx.frontbuffer`: Hardware linear framebuffer mapped from PCI/VBE aperture.
  - `ctx.backbuffer`: Contiguous kernel heap memory tightly packed for tear-free double-buffered rendering.
  - `ctx.wallpaper_buffer`: Precomputed 32-bit RGB background gradient buffer updated only on theme/wallpaper change, enabling 60 FPS compositing without per-frame gradient calculation.
- **Primitives**:
  - `gfx_draw_pixel` & `gfx_draw_pixel_alpha`: Pixel-level drawing with 8-bit alpha blending ($dst = \frac{src \cdot \alpha + dst \cdot (255 - \alpha)}{255}$).
  - `gfx_draw_rect` & `gfx_draw_rect_alpha`: Fast horizontal scanline fills with clipping.
  - `gfx_draw_rounded_rect` & `gfx_draw_rounded_rect_alpha`: Antialiased corner-radius filled rectangles.
  - `gfx_draw_rounded_rect_outline`: Border and chrome drawing.
  - `gfx_draw_circle`: Midpoint circle algorithm for badges, traffic lights, and radio swatches.
  - `gfx_draw_shadow`: Gaussian-like soft drop shadow box with configurable radius, spread, and opacity.
  - `gfx_set_clip` & `gfx_reset_clip`: Viewport clipping bounds stack for window contents.

## 2. Existing Framebuffer Implementation
- **Hardware Interface**: VBE Dispi (Bochs / QEMU / VirtualBox) accessed via I/O ports `0x01CE` (Index) and `0x01CF` (Data).
- **Default Mode**: 1024 x 768 @ 32 bpp, linear framebuffer enabled (`VBE_DISPI_LFB_ENABLED = 0x40`).
- **Memory Mapping**: PCI BAR0 / VBE LFB address discovered at boot and mapped through kernel 4-level PML4 paging.

## 3. Existing Font Implementation
- **Files**: `src/include/font.h`, `src/include/font8x16.h`, `src/kernel/font.c`.
- **Glyph Set**: Embedded 8x16 monospace system font covering ASCII 0..127 with subpixel-like anti-aliasing.
- **Sizes**:
  - `FONT_SIZE_SMALL` (~6x12 / compact)
  - `FONT_SIZE_REGULAR` (8x16 standard base)
  - `FONT_SIZE_LARGE` (12x24 / 1.5x)
  - `FONT_SIZE_TITLE` (16x32 / 2.0x)
- **Measurement & Drawing**: `font_measure_text_width()`, `font_measure_text_height()`, `font_draw_text()`, `font_draw_text_center()`, `font_draw_text_clipped()`.

## 4. Existing Icon Implementation
- **Files**: `src/include/icons.h`, `src/include/icons_assets.h`, `src/drivers/icons.c`, `src/drivers/icons_assets.c`.
- **Themes**:
  - `ICON_THEME_WHITESUR`: Modern macOS Big Sur / Monterey / Ventura squircle icons with detailed gradients.
  - `ICON_THEME_MACOS_TAHOE`: macOS Tahoe / Sequoia design with hyper-smooth corner radii and subtle highlights.
  - `ICON_THEME_VECTOR`: Retro vector outline fallback icons.
- **Rendering**: Full ARGB alpha blending, smooth corner curves, and dynamic scaling blitter `icon_draw_scaled(icon_id, x, y, size)` supporting arbitrary icon dimensions from 24px to 64px.

## 5. Existing Window Manager
- **Files**: `src/include/gui.h`, `src/kernel/gui.c`.
- **Data Structure**: `window_t windows[MAX_WINDOWS]` tracking window position, bounds, active focus, minimize, drag state, and view-content callbacks.
- **Window Chrome**:
  - macOS-style Titlebar (height 28px) with centered text.
  - Traffic lights: Red Close (`COLOR_MAC_CLOSE`), Yellow Minimize (`COLOR_MAC_MIN`), Green Zoom/Expand (`COLOR_MAC_ZOOM`).
  - Active vs Inactive state styling (inactive desaturates traffic lights and dims borders/title text).
  - Soft drop shadows (`gfx_draw_shadow`).
  - Translucency support (`gfx_draw_rounded_rect_alpha`).
  - Viewport clipping per window (`gfx_set_clip`).

## 6. Existing Dock
- **Files**: `src/include/dock.h`, `src/kernel/dock.c`.
- **Features**:
  - macOS-style floating dock centered along the bottom screen edge.
  - Configurable icon dimensions (`32px` to `64px`) and spacing (`2px` to `16px`).
  - Dynamic magnification with distance decay around cursor position.
  - Running application indicators (accent-colored glowing dots).
  - Trash can separation divider and pinned system applications.

## 7. Existing Menu Bar
- **Files**: `src/include/menubar.h`, `src/kernel/menubar.c`.
- **Features**:
  - 24px top status bar with frosted glass alpha translucency.
  -  Apple logo popup menu with system actions (About, Settings, Sleep, Restart, Shutdown).
  - Active application title and standard menus (File, Edit, View, Window, Help).
  - Right-side status extras: Wi-Fi, Battery, Spotlight Search, Control Center, and CMOS RTC Clock.

## 8. Existing System Settings
- **Files**: `src/include/settings.h`, `src/kernel/settings.c`, `src/kernel/gui.c`.
- **Architecture**: 520x350 window with left macOS-style navigation sidebar and right content panel.
- **Current Tabs**:
  - Tab 0: Appearance (Theme preset, accent color, UI scaling, font size, translucency toggle, shadow toggle, icon theme switcher).
  - Tab 1: Displays (Resolution info, color space, UI scaling segmented control).
  - Tab 2: Dock & Icons (Active icon theme, dock icon size, spacing, desktop icon toggle, desktop icon size, icon labels toggle, dock magnify toggle, live preview).
  - Tab 3: Wallpaper (Wallpaper preset info).
  - Tab 4: General (System version, kernel version, storage backend).
- **Interactive Controls**: `src/kernel/ui_controls.c` (Buttons, segmented controls, toggles, checkboxes, sliders, sidebars).

## 9. Existing Configuration System
- **File**: `/etc/ratana/settings.conf` stored on VFS.
- **Format**: Simple key=value plain text parser with checksum protection and automatic fallback to defaults (`settings_reset_defaults()`).

## 10. Files to be Modified
1. `src/include/theme.h`: Add semantic theme tokens, theme profile types, window style types, and dock style types.
2. `src/kernel/theme.c`: Implement complete theme presets (WhiteSur Light/Dark, MacTahoe Light/Dark, Vector, Custom), window styles, dock styles.
3. `src/include/font.h` & `src/kernel/font.c`: Add semantic typography roles (`FONT_ROLE_CAPTION`, `FONT_ROLE_BODY`, etc.) and font scaling factor.
4. `src/include/dock.h` & `src/kernel/dock.c`: Support dock positioning (Bottom, Left, Right), styles, and auto-hide.
5. `src/include/settings.h` & `src/kernel/settings.c`: Expand configuration persistence for window styles, dock position, dock style, auto-hide, menu bar style.
6. `src/kernel/gui.c`: Upgrade System Settings UI to cover all categories (Appearance, Theme, Display, Fonts, Windows, Dock, Menu Bar, Icons, About) with live previews.

## 11. Files to be Added
- `docs/THEMES.md`: Theme presets and visual tokens guide.
- `docs/FONTS.md`: Typography hierarchy and scaling formula.
- `docs/UI_SCALING.md`: Logical-to-physical coordinate transformation guide.
- `docs/WINDOW_STYLE.md`: Window decorations, radii, shadows, and transparency.
- `docs/DOCK.md`: Dock styles, positions, magnification, and auto-hide behavior.
- `docs/SYSTEM_SETTINGS.md`: System Settings user manual and configuration keys reference.

## 12. Risks & Mitigations
- **Rendering Lag**: Expensive full-screen redraws or unoptimized alpha blending could reduce framerate.
  - *Mitigation*: Leverage the existing precomputed wallpaper buffer and dirty rect / clipping optimizations.
- **Scale Overflow**: Integer rounding when computing UI scale could produce 0px elements or divide-by-zero.
  - *Mitigation*: Clamp all scaling factors to valid bounds (`scaled(v) = (v * scale + 50) / 100` with minimum size of 1px).
- **Configuration Corruption**: Truncated or malformed `/etc/ratana/settings.conf`.
  - *Mitigation*: Checksum validation already in place; fall back gracefully to factory defaults if checksum fails or values are out-of-range.

## 13. Implementation Order
- **Phase 1**: Source Audit & Report (Completed).
- **Phase 2**: Central ThemeConfig & Semantic Tokens (`src/include/theme.h`).
- **Phase 3**: Typography & Semantic Font Roles (`src/include/font.h`, `src/kernel/font.c`).
- **Phase 4**: Global UI Scaling Pipeline (`ui_scale_val`).
- **Phase 5**: Light / Dark Mode & Theme Profiles (WhiteSur, MacTahoe, Vector).
- **Phase 6**: Window Style Engine (Radius, Shadows, Transparency, Borders).
- **Phase 7**: Dock Style Engine (Positions, Styles, Magnification, Auto-Hide).
- **Phase 8**: Menu Bar Theming (Height, Background, Transparency).
- **Phase 9**: System Settings UI Expansion (All categories with live previews).
- **Phase 10**: Configuration Persistence (`/etc/ratana/settings.conf`).
- **Phase 11**: Build, QEMU Validation & Documentation.

## 14. Build & Test Plan
- `make clean && make`: Build RatanaOS kernel binary and ELF32 container.
- `make test`: Run native 64-bit ELF layout and kernel symbol verification.
- `qemu-system-x86_64`: Boot graphical desktop in QEMU, launch System Settings, cycle themes, switch icon themes, adjust UI scale and font sizes, verify persistence across reboot.
