# RatanaOS Hardware Enablement & Compatibility Guide

RatanaOS provides comprehensive out-of-the-box hardware compatibility for modern desktops, laptops, workstations, and virtualized systems using an **Open-Source First** strategy combined with standard Linux kernel drivers and user consent policies.

---

## 1. Subsystem Support Overview

| Subsystem Category | Supported Features & Technology | Primary Kernel Drivers & Services |
|---|---|---|
| **Boot Support** | Legacy BIOS, UEFI, GPT, MBR, Secure Boot (Optional), USB Boot, Live USB, NVMe Boot | GRUB2 (UEFI + BIOS Hybrid), `isohybrid` |
| **Storage** | SATA SSD, NVMe SSD, eMMC, HDD, USB Storage, RAID, LVM, LUKS FDE, Btrfs, ext4, XFS | `ahci`, `nvme`, `sdhci`, `uas`, `dm-crypt`, `lvm2`, `mdadm` |
| **CPU Support** | Intel Core/Xeon, AMD Ryzen/EPYC (x86_64 architecture) | Linux Kernel + `intel-microcode` / `amd64-microcode` |
| **Graphics (GPU)** | AMD Radeon, Intel HD/Iris/Arc, NVIDIA GeForce, VirtIO/VMware KMS | `amdgpu`, `i915`/`xe`, `nouveau` (Default), `nvidia-driver` (Optional) |
| **Network** | Gigabit/10G Ethernet, Wi-Fi 5/6/6E/7, Bluetooth 5.x | NetworkManager, `firmware-iwlwifi`, `firmware-realtek`, `bluez` |
| **Audio Stack** | High Definition Audio, Sound Open Firmware (SOF), USB Audio | PipeWire + WirePlumber + `firmware-sof-signed` |
| **Input Devices** | Keyboard, Mouse, Multi-touch Touchpad, Touchscreen, Game Controllers | `libinput`, `evdev`, `hidraw` |
| **Display** | 1080p, 1440p, 4K, Multi-Monitor, HiDPI Scaling (100%-200%) | DRM / KMS / `xrandr` / Wayland |
| **Printers** | HP, Canon, Epson, Brother, Network / USB Printing | CUPS Scheduler + `hplip` + `printer-driver-all` |
| **Cameras** | Integrated USB Webcams, External UVC Webcams | Linux V4L2 (`uvcvideo` / `/dev/video*`) |
| **Power & Laptop** | Battery status, Power profiles, Suspend, Hibernate | `power-profiles-daemon`, `TLP`, `systemctl suspend` |

---

## 2. Detailed Subsystem Specifications

### Boot & Firmware Support
- **Legacy BIOS & UEFI**: Hybrid ISO partition headers allow booting from legacy MBR BIOS systems as well as modern UEFI platforms.
- **GPT & MBR**: Full support for GUID Partition Tables (GPT) and legacy Master Boot Record (MBR).
- **Secure Boot**: Optional UEFI Secure Boot support using signed EFI binaries (`grub-efi-amd64-signed` & Shim).
- **NVMe & USB Boot**: Booting directly from high-speed PCIe NVMe SSDs, external USB drives, or live media environments.

### Storage & Filesystem Subsystems
- **NVMe & SATA**: Full performance enablement for NVMe Express SSDs (`/dev/nvmeXn1`) and standard AHCI SATA drives (`/dev/sdX`).
- **eMMC & USB**: Embedded flash memory (`/dev/mmcblkX`) and USB mass storage controllers.
- **Enterprise Storage**: Support for software RAID (`mdadm`), Logical Volume Manager (`lvm2`), and Full Disk Encryption via `dm-crypt` LUKS2.
- **Filesystems**: Native support for `ext4` (default), `Btrfs` (with copy-on-write snapshots), `XFS` (high throughput), `f2fs`, and `vfat`.

### CPU Architecture Policy
- **Primary Architecture**: `x86_64` (64-bit AMD and Intel processors).
- **Architecture Validation**: RatanaOS hardware tools automatically inspect system architecture (`uname -m`). If a non-x86_64 architecture (such as ARM64 or 32-bit i686) is detected, `ratana-hardware-probe` and `ratana-driver-manager` output explicit unsupported architecture diagnostics rather than failing silently.

### Graphics & Display Engine
- **AMD Radeon**: Open-source `amdgpu` driver with Mesa RADV Vulkan and VA-API hardware video acceleration.
- **Intel Graphics**: Open-source `i915` / `xe` drivers with Iris OpenGL/Vulkan drivers and Intel Media Driver for video decoding.
- **NVIDIA GeForce**:
  - **Default**: Pre-installed open-source `nouveau` driver for high stability, multi-monitor display, and desktop compositor rendering.
  - **Optional Proprietary Driver**: Clear, consent-based workflow via `ratana-driver-manager --install-nvidia` to install official `nvidia-driver` packages for 3D gaming and CUDA acceleration.
- **Display & HiDPI**: Automatic display discovery with support for multi-monitor setups and fractional HiDPI scaling (125%, 150%, 200%).

### Network & Communications
- **Ethernet**: Out-of-the-box support for Intel, Realtek, Broadcom, and Marvell Gigabit/10G network controllers.
- **Wi-Fi**: Staged firmware packages for Intel (`firmware-iwlwifi`), Realtek (`firmware-realtek`), Qualcomm Atheros (`firmware-atheros`), and Broadcom (`firmware-brcm80211`).
- **Bluetooth**: `bluez` stack for pairing wireless keyboards, mice, headsets, and controllers.

### Audio & Microphone Subsystem
- **PipeWire Engine**: Low-latency PipeWire audio server and WirePlumber session manager replacing legacy PulseAudio.
- **Laptop Microphones**: Pre-installed `firmware-sof-signed` (Sound Open Firmware) for modern laptop digital microphone arrays.
- **Audio Diagnostics**: Integrated verification checks for audio playback sinks, microphone sources, and master volume controls.

### Peripherals & Power Management
- **Input Devices**: `libinput` handles multi-touch gestures, palm rejection on touchpads, touchscreen digitizers, and basic game controller HID detection.
- **Printers**: Local USB and network printing powered by CUPS, HPLIP, and standard Linux printing backends.
- **Webcams**: Standard Linux V4L2 (`uvcvideo`) subsystem for immediate video conferencing compatibility.
- **Power Management**: `power-profiles-daemon` integration for Power-Saver, Balanced, and Performance power profiles on laptops, with S3/S0ix suspend and swap hibernation support.

---

## 3. Hardware Diagnostic & Management Tools

RatanaOS includes two dedicated CLI & management utilities:

### 1. `ratana-hardware-probe`
Scans all 12 system hardware categories, verifies active drivers, and generates a formatted audit report saved to `HARDWARE_REPORT.md`.
```bash
# Run hardware probe audit
ratana-hardware-probe
```

### 2. `ratana-driver-manager`
Scans system drivers, identifies missing firmware, logs hardware state, and allows optional proprietary driver installation with explicit user consent.
```bash
# Run driver audit scan
ratana-driver-manager --scan

# Check missing firmware or optional driver packages
ratana-driver-manager --check-missing

# Interactively review and install proprietary NVIDIA drivers
ratana-driver-manager --install-nvidia

# Output driver audit in JSON format
ratana-driver-manager --json
```
