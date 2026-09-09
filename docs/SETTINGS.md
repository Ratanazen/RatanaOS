# Settings & Icon Configuration

The system configuration manager centralizes UI themes, independent icon theme suites, accent colors, global UI scale, font sizes, window effects (translucency, shadows, corner radii), and the full Icon Configuration subsystem. `settings_apply` applies all values to their active subsystems.

## Icon Configuration Subsystem (`icon_config_t`)

The icon configuration subsystem provides centralized control over icon rendering across the desktop and dock:
- **Active Theme Suite**: WhiteSur (macOS Big Sur/Monterey), MacTahoe (macOS Sequoia), and Vector (retro algorithmic squircle fallback).
- **Dock Metrics**: Dynamic dock icon sizing (32 to 64 px), spacing (2 to 16 px), and hover magnification.
- **Desktop Icons**: Toggleable desktop icon visibility (`show_desktop_icons`), desktop icon sizing (32, 48, 64 px), and icon text label visibility.
- **Scaling Blitter**: In-kernel 32-bit ARGB fixed-point blitter (`gfx_draw_icon_rgba_scaled` and `icon_draw_scaled`) provides smooth, artifact-free dynamic scaling without modifying pre-rendered static asset tables.

## System Settings GUI & Shell Commands

- **System Settings.app**: Interactive sidebar tab navigation with a dedicated **Dock & Icons** panel featuring real-time segmented pickers, toggle switches, and a live scaled icon preview suite.
- **Shell CLI (`icons`)**:
  - `icons config`: Displays complete icon configuration parameters and blitter status.
  - `icons theme <whitesur|mactahoe|vector|next>`: Switches active icon theme.
  - `icons size <32..64>`: Configures dock icon size.
  - `icons spacing <2..16>`: Configures dock item spacing.
  - `icons desktop <on|off>`: Toggles desktop drive and folder icons.
  - `icons desktop-size <32|48|64>`: Configures desktop icon size.
  - `icons labels <on|off>`: Toggles desktop text labels.
  - `icons mag <on|off>`: Toggles dock hover magnification.
  - `icons reset`: Restores icon subsystem to macOS default metrics.

## Persistence Boundary

`settings_save` and `settings_load` utilize a CRC32 checksum-protected volatile memory backend. They persist for the kernel execution lifetime; no disk or NVRAM persistence is implemented. `settings_get_backend_name` and `settings_is_persistent` expose that limitation explicitly.

