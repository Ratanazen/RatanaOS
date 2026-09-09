# 🛠️ RatanaOS Live Distribution Build Guide

This document explains the official **RatanaOS Debian Live Distribution** build system, its architecture, and configuration choices.

---

## 🏛️ System Architecture

RatanaOS is built using Debian's standard `live-build` framework targeting **Debian 12 "Bookworm"** (64-bit amd64) with native Linux kernel 6.x and macOS Sequoia visual styling:

- **Upstream Base**: Debian GNU/Linux 12 (Bookworm)
- **Init & Service Manager**: systemd
- **Package Management**: APT / dpkg (`ID_LIKE=debian`)
- **Desktop Environment**: XFCE4 + LightDM
- **Application Dock**: Plank Dock with macOS Sequoia styling
- **Window Controls**: macOS traffic-light buttons (`#FF5F56`, `#FFBD2E`, `#27C93F`) on left (`CHM|`)
- **Icon Suites**: Native WhiteSur (Dark) and MacTahoe (Light) FreeDesktop themes
- **Bootloader & Splash**: GRUB 2 (Hybrid ISO/EFI/BIOS) + Plymouth boot splash
- **System Installer**: Calamares Graphical Installer

---

## 📂 Live-Build Tree Structure

```text
ratanaos-live/
├── auto/
│   └── config                       # lb config automated parameters
├── config/
│   ├── binary                       # ISO binary output settings
│   ├── bootstrap                    # Debootstrap distribution & mirror flags
│   ├── chroot                       # Target chroot environment settings
│   ├── common                       # Architecture and project metadata
│   ├── package-lists/
│   │   └── ratanaos.list.chroot     # Desktop, firmware, drivers, apps
│   ├── includes.chroot/             # Root filesystem overlay
│   │   ├── etc/
│   │   │   ├── os-release           # ID=ratanaos, ID_LIKE=debian
│   │   │   ├── lightdm/             # Autologin configuration
│   │   │   ├── calamares/           # Calamares installer configuration
│   │   │   └── skel/.config/        # XFCE4, Plank, window manager presets
│   │   └── usr/
│   │       ├── local/bin/           # ratanaos-theme-switch script
│   │       ├── share/icons/         # WhiteSur and MacTahoe icon suites
│   │       ├── share/themes/        # RatanaOS-Dark and RatanaOS-Light
│   │       └── share/plymouth/      # Boot splash theme
│   └── includes.binary/
│       └── boot/grub/themes/        # RatanaOS GRUB theme
```

---

## ⚙️ Configuration Choices Rationale

1. **Option A: XFCE4 + Plank**:
   - Provides full macOS desktop fidelity with high performance and low memory consumption (~400MB idle RAM).
   - Fully compatible with standard FreeDesktop specifications, multi-monitor setups, and Calamares installer.
2. **Firmware & Drivers**:
   - Includes `firmware-linux`, `firmware-realtek`, `firmware-atheros`, and non-free firmware packages to ensure immediate compatibility across Intel, AMD, and Realtek hardware.
3. **PipeWire & WirePlumber**:
   - Replaces legacy PulseAudio with PipeWire and WirePlumber for low-latency modern audio and screen sharing.
4. **Reproducible Containerized Build**:
   - Uses `tools/docker-live-build.sh` to run `live-build` inside a pristine Debian Bookworm container (`ratanaos-live-builder`), avoiding host distribution contamination and sudo password requirements.

---

## 🚀 Building the Live ISO

### Prerequisites
- Docker (Docker Engine or Docker Desktop)
- QEMU (`qemu-system-x86_64`) for testing

### Step 1: Configure
```bash
./tools/docker-live-build.sh config
```

### Step 2: Build
```bash
./tools/docker-live-build.sh build
```
The output hybrid ISO image will be generated at `build/ratanaos-live-amd64.hybrid.iso`.

### Step 3: Run & Test in QEMU
```bash
make live-run
# Or manually:
qemu-system-x86_64 -cdrom build/ratanaos-live-amd64.hybrid.iso \
  -m 2G -smp 2 -enable-kvm -net nic,model=virtio \
  -net user -vga virtio
```
