# 🔍 RatanaOS Deviations from Stock Debian Behavior

This document tracks all intentional architectural and configuration differences between standard Debian GNU/Linux 12 ("Bookworm") and RatanaOS.

---

## 📋 Architectural Alignment Principles

1. **Kernel & Drivers**: 100% stock Debian Bookworm Linux kernel 6.1 and modules. No out-of-tree kernel patches or custom syscall ABI in the shipping OS.
2. **APT Repositories**: Debian official mirrors (`main`, `contrib`, `non-free`, `non-free-firmware`) remain unmodified. Third-party software installs via standard `.deb` or `apt` commands without repository breakage.
3. **Identity Compatibility**: `ID_LIKE=debian` is strictly preserved in `/etc/os-release` so hardware detection tools and package scripts recognize the system as Debian.

---

## 🛠️ List of Intentional Deviations

| Component | Stock Debian Bookworm Behavior | RatanaOS Modified Behavior | Motivation / Rationale |
| :--- | :--- | :--- | :--- |
| **System Identity** | `NAME="Debian GNU/Linux"`, `ID=debian` | `NAME="RatanaOS"`, `ID=ratanaos`, `ID_LIKE=debian` | Distro branding while maintaining 100% tooling compatibility |
| **Desktop Environment** | Stock XFCE4 panel on bottom, stock Greybird GTK theme | Top panel (24px, global menu style), centered Plank dock at bottom | macOS Sequoia visual experience and streamlined desktop workflow |
| **Window Manager Controls** | Right-aligned minimize, maximize, close buttons | Left-aligned traffic lights (`CHM\|`: `#FF5F56`, `#FFBD2E`, `#27C93F`) | macOS window control muscle memory |
| **Default Theme** | Greybird GTK + Adwaita icons | `RatanaOS-Dark` / `RatanaOS-Light` GTK3/XFWM + `WhiteSur` / `MacTahoe` icons | High-fidelity dark/light aesthetic matching native RatanaOS design |
| **Live Session Autologin** | Manual login prompt or generic live user | Automatic LightDM graphical login into live user desktop session | Immediate out-of-the-box live testing experience |
| **System Installer** | Debian Installer (curses/ncurses d-i) | Calamares Graphical Installer with RatanaOS branding | Modern, intuitive graphical installation with automated disk partitioning |
| **Boot Splash** | Text or Debian swirl Plymouth theme | Custom RatanaOS dark boot splash with centered emblem | Seamless branded startup and shutdown sequence |
| **Audio Subsystem** | Legacy setups often default to PulseAudio | PipeWire + WirePlumber enabled by default | Low-latency audio and modern screen capture capabilities |
| **Hardware Firmware** | `main` only by default in minimal Debian installs | `non-free` and `non-free-firmware` enabled out-of-the-box | Broad out-of-the-box hardware compatibility (Intel/Realtek/Broadcom Wi-Fi, GPUs) |

---

## 🔧 Upstream Rebase & Maintenance
To update RatanaOS to a new Debian point release or newer release (e.g. Trixie):
1. Update `--distribution` in `ratanaos-live/auto/config`.
2. Re-run `tools/docker-live-build.sh config && tools/docker-live-build.sh build`.
3. Test against `docs/PARITY_CHECKLIST.md`.
