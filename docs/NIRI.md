# RatanaOS Niri Desktop Suite

## Overview
Niri is an infinite horizontal scrollable tiling Wayland compositor. RatanaOS provides a native KDL configuration styled to match the RatanaOS macOS Sequoia visual identity.

---

## 1. Directory Structure
```
~/.config/niri/
├── config.kdl      # Master configuration
├── layout.kdl      # Column gaps, window borders, and focus ring
├── appearance.kdl  # Active/inactive border colors and radii
├── startup.kdl     # Auto-start daemons (Waybar, SwayNC, SWWW)
└── keybinds.kdl    # Scrolling tiling shortcuts
```

---

## 2. Key Shortcuts

| Key Combination | Action |
| :--- | :--- |
| `Super + Return` | Open Kitty Terminal |
| `Super + Space` | Application Launcher |
| `Super + Q` | Close window |
| `Super + Left / Right` | Focus column left / right |
| `Super + Up / Down` | Focus window up / down in column |
| `Super + Shift + Left / Right` | Move column left / right |
| `Super + R` | Switch column preset width |
| `Super + Shift + F` | Fullscreen column |
| `Super + L` | Lock screen |
