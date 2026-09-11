# RatanaOS — Niri Guide

## Overview
Niri is an infinite horizontal scrolling Wayland window manager where windows are arranged in dynamic columns along a continuous ribbon.

## Configuration Structure
- `config.kdl`: Master KDL file defining inputs, window rules, and module includes.
- `layout.kdl`: Preset column widths (33%, 50%, 66%, 100%), 12px gaps, and 2px active focus ring.
- `appearance.kdl`: Theme variables, cursor sizing, and animation slowdown controls.
- `startup.kdl`: Auto-start for Waybar, SwayNC, and theme engine.
- `keybinds.kdl`: Mod+Left/Right column navigation, column resizing, and application spawning.
