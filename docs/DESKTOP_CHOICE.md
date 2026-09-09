# 🖥️ RatanaOS Desktop Environment Decision Matrix (Phase 0)

This document records the architectural evaluation and decision for the desktop environment of the Debian-based RatanaOS distribution.

---

## 📊 Evaluation Matrix

| Evaluation Criteria | Option A: XFCE4 + Plank (Selected) | Option B: Hyprland / Sway + Waybar | Option C: Custom X11/Wayland Port |
| :--- | :--- | :--- | :--- |
| **Time to working result** | **Days** (Rapid integration) | 1–2 weeks | Months (Requires building compositor/display server) |
| **Visual fidelity to macOS spec** | **Medium-High** (Via WhiteSur GTK, custom XFWM4 buttons, Plank) | Medium-High (CSS-based, tiling-centric) | **Highest** (Pixel-for-pixel port of custom C compositor) |
| **Maintenance burden** | **Low** (Maintained upstream by Debian & XFCE community) | Medium (Wayland protocols evolving rapidly) | **High** (You own every pixel, event loop, and bug) |
| **Hardware & Driver Compatibility** | **Full** (Mature X11 stack, works flawlessly on Intel, AMD, NVIDIA, VMs) | Full (Modern GPUs, potential friction with older NVIDIA/VMs) | Unknown (Depends on graphics backend implemented) |
| **Accessibility Tooling (a11y)** | **Full** (Orca screen reader, high contrast, at-spi2 mature) | Partial (Wayland a11y protocols still developing) | **None** (Must implement from scratch) |
| **Community Theme Resources** | **Abundant** (Extensive GTK3/XFWM WhiteSur ecosystem) | Growing (Hyprland dotfiles, Waybar CSS) | **None** (Bespoke internal format only) |

---

## 🎯 Formal Decision: Option A (XFCE4 + Plank)

### Rationale:
1. **Inherited Stability & Driver Coverage**: XFCE runs reliably across all hardware (modern laptops, desktops, and virtual machines in QEMU, VMware, VirtualBox).
2. **Standard FreeDesktop Tooling**: Full compatibility with Calamares GUI installer, LightDM display manager, and standard system tray indicators (`nm-applet`, `volumeicon`, `xfce4-power-manager`).
3. **macOS Workflow Parity**:
   - **Top Panel**: Clean 24px panel hosting an Apple-style Whisker application menu, window title, system indicators, and a formatted clock (`MM/DD HH:MM`).
   - **Bottom Dock**: Plank dock configured with floating translucent glass styling (alpha 210/255), 48px icons, and magnification on hover.
   - **Window Controls**: Custom XFWM4 theme providing macOS traffic-light buttons on the top left (`#FF5F56` close, `#FFBD2E` minimize, `#27C93F` zoom/maximize).
4. **Low Resource Footprint**: Idle memory consumption is ~350–450 MB RAM, leaving maximum host resources available for user applications.

---

## 📌 Documented Tradeoffs & Upgrade Path
- While Option C offers native source-level continuity with the custom microkernel GUI (`src/kernel/gui/`), the maintenance overhead of maintaining a custom X11/Wayland window server would divert effort away from day-to-day usability.
- Wayland (Option B) remains an attractive future target once Debian's Wayland ecosystem and NVIDIA driver packaging achieve parity with X11 across all target hardware.
