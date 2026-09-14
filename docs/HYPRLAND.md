# RiOS Hyprland Desktop Suite

## Overview
RiOS features a modular, riced Hyprland setup with dynamic tiling, rounded corners, drop shadows, Gaussian blur, and fluid gestures.

---

## 1. Directory Structure
```
~/.config/hypr/
├── hyprland.conf      # Master configuration orchestrator
├── monitors.conf      # Display resolutions and refresh rates
├── environment.conf   # Wayland and NVIDIA environment variables
├── startup.conf       # Auto-start daemons (Waybar, SwayNC, SWWW)
├── animations.conf    # Cubic-bezier animation curves
├── rules.conf         # Window rules and workspace bindings
└── keybinds.conf      # Shortcuts and media control keys
```

---

## 2. Key Shortcuts

| Key Combination | Action |
| :--- | :--- |
| `Super + Return` | Open Kitty Terminal |
| `Super + Space` | Application Launcher (Wofi / Rofi) |
| `Super + Q` | Close active window |
| `Super + E` | Open File Manager (Thunar) |
| `Super + B` | Open Web Browser (Firefox) |
| `Super + V` | Clipboard history manager |
| `Super + F` | Toggle fullscreen |
| `Super + T` | Toggle floating mode |
| `Super + L` | Lock screen (`ri-lock`) |
| `Super + Shift + S` | Interactive area screenshot (`grim` + `slurp`) |
| `Print` | Full screen capture |
| `Super + Shift + R` | Screen recording toggle (`wf-recorder`) |
| `Super + 1..9` | Switch to workspace 1..9 |
| `Super + Shift + 1..9` | Move active window to workspace 1..9 |
