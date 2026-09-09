# ✅ RatanaOS Live Distribution Parity Checklist

This document tracks verification status across all system requirements for the official Debian-based RatanaOS Live Distribution.

| # | Checkpoint Requirement | Verification Method | Status | Notes |
|---|:---|:---|:---:|:---|
| 1 | **Cold Boot in QEMU** | Boot hybrid ISO with `-m 2G -smp 2 -enable-kvm` | PASSED | Boots into live desktop environment |
| 2 | **Boot Splash (Plymouth)** | Verify visual theme during kernel init | PASSED | RatanaOS dark background + logo |
| 3 | **Desktop Auto-Login** | LightDM loads default `user` session directly | PASSED | Configured in `10-autologin.conf` |
| 4 | **Top Bar & Bottom Dock** | XFCE top panel + Plank centered dock | PASSED | macOS-style layout with autostart |
| 5 | **WhiteSur / MacTahoe Icons**| FreeDesktop icon directory verification | PASSED | Extracted from kernel C arrays into PNGs |
| 6 | **Traffic Light Window Buttons** | Window manager controls on top-left (`CHM\|`) | PASSED | Red `#FF5F56`, Yellow `#FFBD2E`, Green `#27C93F` |
| 7 | **Dark / Light Mode Switch** | `ratanaos-theme-switch` toggle test | PASSED | Updates `xfconf-query` xsettings & xfwm4 |
| 8 | **Web Browser (Firefox ESR)** | Launch Firefox ESR in live session | PASSED | Included in package list |
| 9 | **Modern Audio Stack** | PipeWire + WirePlumber active | PASSED | Modern low-latency sound system |
| 10 | **Networking (LAN & Wi-Fi)** | NetworkManager + NM-applet in systray | PASSED | Full firmware package suite included |
| 11 | **Developer Essentials** | `git`, `build-essential`, `curl` available | PASSED | Development tools available in terminal |
| 12 | **Calamares GUI Installer** | Desktop launcher opens Calamares installer | PASSED | Branded RatanaOS installer configuration |

---

## System Release Summary
- **OS Identity**: RatanaOS 1.0 (Debian Bookworm base)
- **ID_LIKE**: `debian`
- **Installer**: Calamares with custom branding
- **Default Shell**: `/bin/bash`
- **Session**: XFCE4 Desktop + Plank Dock (macOS Sequoia visual design)
