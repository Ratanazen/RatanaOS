# RatanaOS — Hyprland Guide

## Overview
RatanaOS Hyprland is configured with an M4 Pro / macOS Sequoia design language featuring 120Hz ProMotion cubic-bezier curves, rounded corners (16px), acrylic background blur (3 passes), drop shadows, and 3-finger trackpad gestures.

## Modular Config Files
- `hyprland.conf`: Master include file sourcing all sub-components.
- `monitors.conf`: High-refresh rate monitor auto-detection.
- `environment.conf`: Wayland environment flags for Qt, GTK, Firefox, and Electron.
- `startup.conf`: Background services (Waybar, SwayNC, hypridle, swww, theme engine).
- `animations.conf`: Cubic bezier physics curves (`smoothFast`, `appleEase`, `appleBounce`).
- `rules.conf`: Smart window floating, dialog centering, picture-in-picture, and layer blur.
- `keybinds.conf`: Full macOS / Linux rice key bindings.
