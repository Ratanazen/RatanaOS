# 📝 RatanaOS Changelog

All notable changes to the RatanaOS project are documented in this file.

---

## [2.0.0] - 2026-09-09 — Major Architecture Pivot: Debian Live Remaster

### 🚀 Architecture Pivot (Supercedes Custom Microkernel)
- Pivoted RatanaOS from a standalone educational microkernel prototype to a production-usable distribution based on **Debian GNU/Linux 12 ("Bookworm")**, following the architectural approach of Ubuntu, Pop!_OS, and elementary OS.
- Maintained `ID_LIKE=debian` in `/etc/os-release` to ensure zero functional gaps and complete compatibility with upstream Debian packages and APT repositories.
- Preserved all original custom kernel code in `src/` and documented its architecture in `docs/LEGACY_KERNEL.md`.

### 🖥️ Desktop & Theming Layer (macOS Sequoia Experience)
- Implemented **Option A**: XFCE4 + LightDM + Plank Dock as evaluated in `docs/DESKTOP_CHOICE.md`.
- Exported kernel ARGB icon assets into complete multi-resolution FreeDesktop icon suites: `RatanaOS-WhiteSur` (Dark) and `RatanaOS-MacTahoe` (Light) spanning 16x16 up to 256x256.
- Created `RatanaOS-Dark` and `RatanaOS-Light` XFWM/GTK themes with authentic traffic light window buttons (`#FF5F56`, `#FFBD2E`, `#27C93F`) on top left (`CHM|`).
- Configured 24px top panel with Apple menu (``), window title, system status tray, and clock (`MM/DD HH:MM`).
- Configured floating centered Plank dock with 48px icons and magnification.
- Created `ratanaos-theme-switch` script for instant dark/light mode toggling.

### 🔌 Hardware Coverage & Drivers
- Broad hardware support across Intel and AMD CPUs (`intel-microcode`, `amd64-microcode`).
- Intel, AMD, and Nouveau open-source GPU support with Mesa Vulkan acceleration.
- Modern low-latency audio via PipeWire and WirePlumber.
- Full wireless and wired networking firmware (`firmware-linux`, `firmware-realtek`, `firmware-iwlwifi`, `firmware-atheros`).
- Full printing support via CUPS and `printer-driver-all`.

### 📦 Installation & Boot Branding
- Integrated Calamares Graphical Installer with RatanaOS branding and auto-partitioning.
- Custom Plymouth boot splash theme with dark background and emblem.
- Custom GRUB 2 theme matching system aesthetics.
- Login screen theming via LightDM GTK Greeter.

### 🛡️ Security & Compliance
- AppArmor enabled by default.
- Locked root account with sudo-mediated administration.
- Zero open listening ports on clean boot.
- Documented in `docs/SECURITY_BASELINE.md`.

### 🛠️ Build Tooling & Release Engineering
- Containerized `live-build` pipeline via `tools/docker-live-build.sh`.
- Added Makefile targets: `make live-config`, `make live-iso`, `make live-run`, `make live-checksum`, and `make live-clean`.
- Authored `docs/BUILD_LIVE.md`, `docs/DEVIATIONS_FROM_DEBIAN.md`, and `docs/PARITY_CHECKLIST.md`.

---

## [1.0.0] - Earlier Releases — Custom 64-bit Microkernel Prototype
- 64-bit Long Mode Multiboot kernel with 4-level PML4 paging.
- Preemptive round-robin scheduler and Ring 3 task execution.
- Linux x86-64 syscall compatibility gate (MSR LSTAR `0xC0000082`).
- Double-buffered linear framebuffer window compositor and macOS Sequoia desktop GUI.
- Preserved for educational and reference purposes in `docs/LEGACY_KERNEL.md`.
