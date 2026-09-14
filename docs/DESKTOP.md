# RiOS Desktop Architecture

RiOS is engineered with a modular, multi-compositor Wayland architecture supporting **Hyprland**, **Niri**, and **Sway**, with fallback desktop support for **KDE Plasma** and **GNOME**.

## Compositor Selection

Users can choose their preferred session directly from the LightDM login screen or switch at runtime via `ri-session`:

| Session | Compositor | Type | Key Features |
| :--- | :--- | :--- | :--- |
| **RiOS Hyprland** | Hyprland | Dynamic Tiling | 120Hz ProMotion animations, acrylic blur, dual-pass shadows, trackpad gestures |
| **RiOS Niri** | Niri | Infinite Column | Horizontal strip scrolling, modular KDL config, preset column ratios |
| **RiOS Sway** | Sway | Manual Tiling | i3-compatible, ultra-low memory footprint, rock-solid stability |
| **GNOME Desktop** | Mutter | Floating/Stacking | Traditional macOS Sequoia styling with Dash-to-Dock and WhiteSur theme |
| **KDE Plasma** | KWin | Modular Desktop | Full Qt5/Qt6 desktop with custom BreezeDark / WhiteSur left-window controls |

## Directory Hierarchy

- `~/.config/hypr/`: Modular Hyprland configuration (`monitors.conf`, `environment.conf`, `rules.conf`, `animations.conf`, `keybinds.conf`, `startup.conf`).
- `~/.config/niri/`: Niri scrolling WM configuration (`config.kdl`, `layout.kdl`, `appearance.kdl`, `startup.kdl`, `keybinds.kdl`).
- `~/.config/sway/`: Sway configuration (`config`).
- `~/.config/waybar/`: Adaptive Waybar layouts (`config-top.json`, `config-bottom.json`, `config-minimal.json`, `style.css`).
- `~/.config/ri/`: Shared theme engine (`theme.conf`, `wallpapers/`, `backups/`).
