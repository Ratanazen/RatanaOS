# Reference Architecture: Hyprdots Adaptations in RatanaOS

## Overview
This document outlines the architectural patterns, styling principles, and technical approaches adapted from [Senshi111/debian-hyprland-hyprdots](https://github.com/Senshi111/debian-hyprland-hyprdots) into RatanaOS.

RatanaOS maintains its own unique visual identity (THE JIRAIYA branding, macOS Sequoia-inspired aesthetics, 7-theme dynamic engine), while adopting proven Debian Wayland packaging and deployment techniques from the reference repository.

---

## 1. Concepts Reused & Adapted

| Reference Pattern | RatanaOS Adaptation |
| :--- | :--- |
| **Debian Hyprland Compatibility** | Used Debian Bookworm package naming and avoided Arch AUR assumptions. Replaced incompatible packages with Debian-native equivalents (e.g., `fastfetch` -> `neofetch`, `fonts-khmeros-core` -> `fonts-khmeros`). |
| **Modular Hyprland Configuration** | Split `hyprland.conf` into modular sub-configs: `monitors.conf`, `environment.conf`, `startup.conf`, `animations.conf`, `rules.conf`, `keybinds.conf`. |
| **Waybar Ricing & Multi-Layout** | Adapted JSON layout structure and dynamic CSS theme variables (`theme.css`) to allow instant theme switching without restarting the compositor. |
| **SwayNC Notification Control** | Reused SwayNC notification center widget styling, integrating it with the RatanaOS theme engine for dark/light/glass/cyber modes. |
| **Safe User Skeletons** | Maintained configuration templates in `/etc/skel/.config/` ensuring user permissions are never root-owned. |
| **Dynamic Wallpaper Management** | Implemented `ratana-wallpaper` supporting category directories and daemon transition management. |

---

## 2. What Was NOT Copied (Protected RatanaOS Functionality)

- **Branding**: No reference logos or wallpapers were copied. All assets feature the official RatanaOS and **THE JIRAIYA** shield identity.
- **Multi-Compositor Support**: Unlike Hyprdots (which focuses solely on Hyprland), RatanaOS natively supports **Hyprland**, **Niri** (scrollable tiling via KDL), and **Sway** (i3 compatibility) under the same unified theme engine.
- **Two-Stage Real OS Installer**: RatanaOS implements a complete disk partitioner and bootloader setup for real PC hardware, whereas dotfile repositories only handle user-space dotfiles.
- **Calamares GUI & Live Build**: RatanaOS preserves its Debian `live-build` hybrid ISO pipeline and Calamares installer sequence.
