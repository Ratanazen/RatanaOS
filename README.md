# RatanaOS — Production Debian-Based macOS Remaster

<div align="center">

![Release](https://img.shields.io/badge/Release-2.0_Sequoia_Edition-blueviolet?style=for-the-badge)
![Base](https://img.shields.io/badge/Base-Debian_12_Bookworm_amd64-crimson?style=for-the-badge)
![Desktop](https://img.shields.io/badge/Desktop-macOS_Sequoia_%2F_XFCE4_%2B_Plank-brightgreen?style=for-the-badge)
![Packaging](https://img.shields.io/badge/Package_Manager-APT_%2F_dpkg-blue?style=for-the-badge)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

**A modern, production-usable 64-bit operating system based on Debian GNU/Linux 12 ("Bookworm"), featuring an authentic macOS Sequoia-style desktop environment, complete hardware and driver support, PipeWire audio, and the Calamares Graphical Installer.**

</div>

---

> [!NOTE]
> **Architecture Transition (v2.0)**:
> RatanaOS's earlier prototype was a custom x86_64 microkernel with a hand-built macOS-style GUI. That work is fully preserved in the repository as an educational/reference project and documented in [`docs/LEGACY_KERNEL.md`](docs/LEGACY_KERNEL.md).
>
> The shipping distribution described in this README is built on the **Debian GNU/Linux 12 Bookworm** foundation (following the architectural model of Ubuntu, Pop!_OS, and elementary OS), delivering zero functional gaps vs stock Linux, full application compatibility, and rock-solid stability.

---

## 🧭 Table of Contents
- [What is RatanaOS?](#-what-is-ratanaos)
- [Key Features](#-key-features)
- [Getting Started](#-getting-started)
- [Building from Source](#-building-from-source)
- [Desktop Layout & Theming](#-desktop-layout--theming)
- [Hardware & Driver Coverage](#-hardware--driver-coverage)
- [Legacy Custom Kernel Reference](#-legacy-custom-kernel-reference)
- [Documentation Index](#-documentation-index)
- [License & Credits](#-license--credits)

---

## 🌟 What is RatanaOS?

RatanaOS is a refined Linux distribution crafted for developers, designers, and enthusiasts who appreciate the aesthetic elegance and desktop workflow of macOS, combined with the power, privacy, and extensive software ecosystem of Debian.

### Architectural Model
Like Ubuntu, Pop!_OS, and Linux Mint, RatanaOS does not reimplement the Linux kernel or low-level userspace from scratch. Instead, it inherits:
- **Upstream Kernel**: Genuine Debian Bookworm Linux kernel 6.1 (x86_64)
- **Service Management**: systemd & udev
- **Package Management**: APT / dpkg (`ID_LIKE=debian` in `/etc/os-release`)
- **Software Repository**: Over 60,000 official Debian packages (`main`, `contrib`, `non-free`, `non-free-firmware`)

On top of this robust foundation, RatanaOS integrates a heavily customized desktop layer delivering macOS Sequoia aesthetics, traffic-light window controls, a floating bottom dock, and standard FreeDesktop icon suites.

---

## ⚡ Key Features

- **macOS Sequoia Visual Layer**:
  - Top 24px global panel with Apple menu (``), window title, system indicators, and `MM/DD HH:MM` clock.
  - Floating centered bottom Plank dock with translucent glass effect and icon magnification on hover.
  - Traffic-light window controls (`#FF5F56` close, `#FFBD2E` minimize, `#27C93F` maximize) positioned on the top left (`CHM|`).
  - Native `RatanaOS-WhiteSur` (Dark) and `RatanaOS-MacTahoe` (Light) FreeDesktop icon themes across all standard resolutions (16x16 up to 256x256).
- **Out-of-the-box Hardware Support**:
  - Full wireless & wired network firmware (`firmware-linux`, `firmware-realtek`, `firmware-iwlwifi`, `firmware-atheros`).
  - Intel, AMD, and Nouveau graphics with Mesa Vulkan and OpenGL acceleration.
  - Modern low-latency audio via PipeWire and WirePlumber.
  - Automatic battery and power management with ACPI and UPower.
- **Calamares GUI Installer**:
  - Simple, automated installer with EFI/BIOS support and partition management.
- **Security Baseline**:
  - AppArmor enabled by default, locked root account, zero open network ports out-of-the-box.

---

## 🚀 Getting Started

### 1. Download & Verify
Download the latest `ratanaos-live-amd64.hybrid.iso` and verify its integrity:
```bash
sha256sum -c RatanaOS.iso.sha256
```

### 2. Boot in QEMU Virtual Machine
To test RatanaOS immediately without touching physical storage:
```bash
qemu-system-x86_64 -m 2048 -smp 2 -enable-kvm \
  -cdrom build/ratanaos-live-amd64.hybrid.iso \
  -netdev user,id=net0 -device virtio-net-pci,netdev=net0 \
  -device virtio-vga -device intel-hda -device hda-duplex
```
Or simply run:
```bash
make live-run
```

### 3. Flash to USB Drive
Write the hybrid ISO directly to your USB thumb drive (replace `/dev/sdX` with your target drive):
```bash
sudo dd if=build/ratanaos-live-amd64.hybrid.iso of=/dev/sdX bs=4M status=progress oflag=sync
```

---

## 🛠️ Building from Source

RatanaOS uses Debian's official `live-build` framework inside a reproducible container environment:

### Prerequisites
- Docker Engine or Docker Desktop
- GNU Make

### Build Steps
```bash
# 1. Clone repository
git clone https://github.com/Ratanazen/RatanaOS.git
cd RatanaOS

# 2. Configure live-build
make live-config

# 3. Build the Live Hybrid ISO image
make live-iso

# 4. Generate SHA256 checksum
make live-checksum
```
The resulting hybrid bootable ISO will be generated at `build/ratanaos-live-amd64.hybrid.iso`.

---

## 🖥️ Desktop Layout & Theming

RatanaOS includes an instant dark/light mode toggle:
```bash
# Switch to Dark Mode (WhiteSur icons + dark GTK)
ratanaos-theme-switch dark

# Switch to Light Mode (MacTahoe icons + light GTK)
ratanaos-theme-switch light
```

---

## 📚 Legacy Custom Kernel Reference

All original source code for the custom 64-bit freestanding kernel, 4-level paging, process scheduler, and custom double-buffered GUI compositor remains intact in the repository:
- Source tree: [`src/`](src/)
- Architecture manual: [`docs/LEGACY_KERNEL.md`](docs/LEGACY_KERNEL.md)

To compile and boot the legacy freestanding kernel in QEMU:
```bash
# Build custom kernel
make

# Run unit tests
make test-all

# Boot in QEMU
make run
```

---

## 📖 Documentation Index

- [`docs/DESKTOP_CHOICE.md`](docs/DESKTOP_CHOICE.md) — Desktop environment evaluation and decision matrix.
- [`docs/BUILD_LIVE.md`](docs/BUILD_LIVE.md) — Complete live-build manual and design choices.
- [`docs/DEVIATIONS_FROM_DEBIAN.md`](docs/DEVIATIONS_FROM_DEBIAN.md) — Upstream Debian comparison and deliberate differences.
- [`docs/SECURITY_BASELINE.md`](docs/SECURITY_BASELINE.md) — System hardening and security configuration.
- [`docs/PARITY_CHECKLIST.md`](docs/PARITY_CHECKLIST.md) — 16-point QA verification and parity scorecard.
- [`docs/LEGACY_KERNEL.md`](docs/LEGACY_KERNEL.md) — Custom microkernel reference manual.

---

## 📜 License & Credits

- **RatanaOS**: Licensed under the [MIT License](LICENSE).
- **Debian Base**: Debian GNU/Linux is a registered trademark of Software in the Public Interest, Inc.
- **Icons & Theme Assets**: Ported and adapted under their respective open-source licenses.
