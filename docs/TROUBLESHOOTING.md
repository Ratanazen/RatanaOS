# RatanaOS — Troubleshooting & Diagnostics

## Common Questions and Fixes

### 1. Audio or Microphone Not Responding
PipeWire handles all audio routing. To restart audio services:
```bash
systemctl --user restart pipewire wireplumber
```

### 2. Waybar Not Displaying After Screen Reconnect
Waybar can be reloaded safely without restarting your session:
```bash
killall -SIGUSR2 waybar
```

### 3. Tuning Performance on Low-End Hardware / Virtual Machines
If running on hardware with limited GPU acceleration or running inside a virtual machine without 3D acceleration, enable the low-end performance profile:
```bash
ratana-performance low
```
This disables window blur, ambient drop shadows, and high-frequency animations.

### 4. Restoring Default Configurations
If any personal desktop configuration becomes damaged, restore from the automated backup:
```bash
ls -la ~/.config/ratana/backups/
```
Or reset default themes and styling:
```bash
ratana-theme dark
```
