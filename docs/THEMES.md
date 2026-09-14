# RiOS 7-Theme Dynamic Engine

## Overview
RiOS includes 7 production-quality visual themes that automatically apply across Compositors (Hyprland, Niri, Sway), Waybar, Launchers (Wofi, Rofi), Notification Centers (SwayNC), Terminals (Kitty, Foot), and GTK/Qt toolkits.

---

## 1. Available Themes

| Theme ID | Style / Accent | Purpose |
| :--- | :--- | :--- |
| `ri-dark` | Deep Slate / Electric Blue (`#3b82f6`) | macOS Sequoia Dark default |
| `ri-light` | Soft Silver / Crisp Azure (`#2563eb`) | High-contrast daytime environment |
| `ri-glass` | Translucent Obsidian / Cyan (`#06b6d4`) | Frosted glass blur aesthetic |
| `ri-cyber` | Void Black / Neon Pink (`#ec4899`) | Cyberpunk aesthetic with glowing borders |
| `ri-anime` | Pastel Midnight / Lavender (`#a855f7`) | Vibrant aesthetic with soft curves |
| `ri-minimal`| Pure Monochrome (`#e2e8f0`) | Zero distraction development |
| `ri-purple` | Royal Purple (`#8b5cf6`) | Modern luxury dark palette |

---

## 2. Switching Themes

Run the switcher command:
```bash
ri-theme <theme-id>
# Examples:
ri-theme cyber
ri-theme glass
ri-theme dark
```
To list all installed themes:
```bash
ri-theme list
```
