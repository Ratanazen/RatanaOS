# RatanaOS Hardware & Installation Guide

This document provides step-by-step instructions for installing and configuring RatanaOS across desktops, laptops, workstations, and virtual machines.

---

## 1. System Requirements

| Specification | Minimum Requirement | Recommended Specification |
|---|---|---|
| **CPU** | 64-bit x86_64 Dual-Core Processor | 64-bit x86_64 Quad-Core Processor (Intel or AMD) |
| **RAM** | 2 GB RAM (Lite Edition) / 4 GB (Standard) | 8 GB RAM or higher |
| **Storage Space** | 20 GB free disk space | 64 GB+ NVMe or SATA SSD |
| **Firmware Mode** | Legacy BIOS or UEFI | UEFI mode with GPT disk formatting |
| **Graphics** | 1024x768 resolution display | Intel, AMD, or NVIDIA GPU with Vulkan/OpenGL acceleration |
| **Network** | Ethernet or Wi-Fi adapter | Broadband internet for initial updates |

> [!IMPORTANT]
> **Architecture Requirement**: RatanaOS v1 requires an `x86_64` (amd64) 64-bit CPU architecture. Non-x86_64 architectures (ARM64, 32-bit x86) are not currently supported.

---

## 2. Boot Modes & Pre-Installation Setup

RatanaOS ISO images use hybrid boot headers compatible with both modern UEFI and legacy BIOS systems.

### A. Live USB Creation
1. **Linux / macOS**: Use `dd` or GNOME Disks:
   ```bash
   sudo dd if=RatanaOS-Standard.iso of=/dev/sdX bs=4M status=progress conv=fsync
   ```
2. **Windows**: Use Rufus (select **DD Image mode**) or BalenaEtcher.

### B. Firmware (BIOS / UEFI) Setup
1. **Boot Mode**: UEFI is recommended for modern laptops and PCs. Legacy BIOS (CSM) is supported for older hardware.
2. **Partition Scheme**:
   - **UEFI Mode**: Select **GPT** (GUID Partition Table) with an EFI System Partition (FAT32, formatted at `/boot/efi`, 512MB–1GB).
   - **Legacy BIOS Mode**: Select **MBR** (Master Boot Record) or GPT with BIOS boot partition.
3. **Secure Boot**: Optional. RatanaOS includes signed kernel & shim components compatible with UEFI Secure Boot.

---

## 3. Storage & Partitioning Options

The RatanaOS installer supports a wide range of storage media and filesystem configurations:

### Storage Media Supported:
- **NVMe SSDs**: High-speed PCIe solid-state drives (`/dev/nvme0n1`).
- **SATA SSDs & HDDs**: Standard AHCI drives (`/dev/sda`).
- **eMMC Storage**: Embedded Flash memory (`/dev/mmcblk0`).
- **USB Drives**: Live USB and external portable installations.

### Advanced Partitioning Features:
- **LUKS Full Disk Encryption**: Encrypt your operating system partition with AES-256 via `dm-crypt`.
- **Logical Volume Manager (LVM)**: Flexible volume allocation and dynamic partition resizing.
- **Software RAID**: RAID 0/1/5/10 configurations via `mdadm`.

### Supported Filesystems:
- **ext4**: Standard, highly reliable Linux filesystem (Default).
- **Btrfs**: Advanced filesystem featuring Copy-on-Write, integrated subvolumes, and instant snapshot rollbacks.
- **XFS**: High-performance enterprise filesystem for large storage volumes.

---

## 4. Post-Installation Driver & Hardware Workflow

After completing system installation and booting into RatanaOS, follow this workflow to audit and manage hardware drivers:

### Step 1: Run Hardware Audit
Open a terminal and run `ratana-hardware-probe` to perform a comprehensive hardware discovery scan and write `HARDWARE_REPORT.md`:
```bash
ratana-hardware-probe
```

### Step 2: Run Driver Manager Scan
Check active drivers, missing packages, and recommended open-source or proprietary drivers:
```bash
ratana-driver-manager --scan
```

### Step 3: Proprietary Driver Installation (Optional)
RatanaOS follows an **Open-Source First** policy. Open-source drivers (`amdgpu`, `i915`, `nouveau`) are active by default.
For NVIDIA GPUs requiring maximum 3D gaming performance or CUDA development:
```bash
ratana-driver-manager --install-nvidia
```
*Note: The Driver Manager will ask for explicit user confirmation before installing proprietary drivers.*

---

## 5. Subsystem Verification Commands

You can verify hardware subsystem functionality post-install:

- **Display & Resolution**: `xrandr` or System Settings -> Display.
- **Network & Wi-Fi**: `nmcli device` or NetworkManager tray applet.
- **Audio & Microphone**: `wpctl status` or `pactl info` (PipeWire Audio Engine).
- **Power & Battery**: `power-profiles-daemon` or `cat /sys/class/power_supply/BAT*/capacity`.
- **Printers**: Open CUPS interface at `http://localhost:631` or run `lpstat -p`.
- **Webcams**: Test using Cheese or V4L2 utility (`v4l2-ctl --list-devices`).

---

## 6. Troubleshooting

- **No Wi-Fi Adapters Visible**: Verify if your laptop requires proprietary firmware. Connect via Ethernet and run `ratana-driver-manager --check-missing`.
- **Black Screen on Boot (NVIDIA GPUs)**: Add `nomodeset` to the GRUB boot line, boot into RatanaOS, and execute `ratana-driver-manager --install-nvidia`.
- **Audio Input / Mic Not Detected**: Ensure `firmware-sof-signed` is installed for modern Intel/AMD laptop digital microphones.
