# RatanaOS v13 Ultimate

[![Build Status](https://github.com/Ratanazen/RatanaOS/actions/workflows/main.yml/badge.svg)](https://github.com/Ratanazen/RatanaOS/actions)
[![Latest Release](https://img.shields.io/github/v/release/Ratanazen/RatanaOS)](https://github.com/Ratanazen/RatanaOS/releases/latest)
[![License: GPL-3.0](https://img.shields.io/badge/License-GPL%203.0-blue.svg)](LICENSE)

Welcome to **RatanaOS v13 Ultimate**, a modular, Debian-based Linux distribution engineered from the ground up for cybersecurity professionals, software developers, system administrators, and daily desktop users. 

RatanaOS provides an original, curated user experience, shipping with a bespoke Qt6-based desktop architecture, hardened security defaults, and a highly modular ISO build system.

---

## 📥 Download

> **[⬇️ Download RatanaOS from the GitHub Releases page](https://github.com/Ratanazen/RatanaOS/releases/latest)**

| Edition | File | Size | Best For |
|---|---|---|---|
| 🪶 Lite | `RatanaOS-Lite.iso` | ≈2 GB | Older hardware, XFCE |
| 🖥️ Standard | `RatanaOS-Standard.iso` | ≈3 GB | Everyday desktop, KDE Plasma |
| 🧑💻 Developer | `RatanaOS-Developer.iso` | ≈3.5 GB | Programming, containers, full toolchains |
| 🔐 Cyber | `RatanaOS-Cyber.iso` | ≈4 GB | Cybersecurity education, penetration testing |
| 🗄️ Server | `RatanaOS-Server.iso` | ≈800 MB | Headless servers, Docker, hardened kernels |
| 🍓 ARM64 | `RatanaOS-ARM64.img` | ≈2 GB | Raspberry Pi 4/5, ARM SBCs |

### Verify Your Download
```bash
# Download the checksum file alongside your ISO, then run:
sha256sum -c SHA256SUMS
```

---

## 🌟 Key Features

- **Four Custom Build Profiles**: Choose between `ratana-lite`, `ratana-standard`, `ratana-developer`, and `ratana-cyber` depending on your hardware and professional needs.
- **Dynamic 13-Step Installer**: A beautiful Qt6 graphical installer that supports Btrfs, LUKS encryption, Timezone selection, Avatar uploading, and on-the-fly "Package Profile" selections so you can install specific cybersecurity or developer toolkits directly from the Live USB.
- **Pre-configured Development Environment**: Out-of-the-box support for GCC, Clang, CMake, Git, Python 3, Rust, Go, Java, Node.js, and Docker.
- **Hardened Security**: Default-deny UFW firewall, enforced AppArmor profiles, and strict Auditd tracking out of the box.
- **Original Qt6 Applications**:
  - **Ratana Terminal**: A GPU-accelerated workspace terminal.
  - **Ratana Software Center**: Unified APT and Flatpak graphical package management with offline repository support.
  - **Ratana Update Manager**: Background daemon for system upgrades and Btrfs snapshot rollbacks.
  - **RatanaAI Assistant**: Built-in AI copilot for shell command explanations and system troubleshooting.

- **Advanced Personalization System**:
  - **Ratana Profile Manager**: Upload avatars and manage local user profiles (`~/.config/ratana/profile/`).
  - **Ratana Appearance Center**: Change themes (Dark, Light, AMOLED), apply icon packs, and manage wallpapers.
  - **Ratana Login Manager**: Customize the login screen background, avatar display, and blur effects.
  - **Ratana Branding Tool**: Administrator utilities to configure OS logos, Plymouth boot animations, and GRUB backgrounds.
  - **Unified Databases**: Settings stored in lightweight SQLite databases (`themes.db`, `profiles.db`, `settings.db`).

---

## 🛠️ How to Build the ISO

The RatanaOS build system utilizes proven Linux tools (`live-build`, `debootstrap`, `mksquashfs`, `grub-mkrescue`) to generate a hybrid UEFI/BIOS bootable Live ISO.

### Prerequisites
You must be running a Debian-based host system with the following packages installed:
```bash
sudo apt update
sudo apt install debootstrap live-build grub-pc-bin grub-efi-amd64-bin xorriso squashfs-tools mtools dosfstools rsync qemu-system-x86 ovmf
```

### Build Instructions
1. Clone the repository and navigate to the root directory.
2. Run the make build command to automatically generate the ISO:
   ```bash
   make build
   ```
3. **Or**, run the ISO builder manually and specify your target profile (`ratana-lite`, `ratana-standard`, `ratana-developer`, `ratana-cyber`):
   ```bash
   ./builder/build-iso.sh ratana-cyber amd64
   ```
4. The build process will bootstrap a Debian base, apply the RatanaOS overlays, generate a SquashFS filesystem, and package it into an ISO.
5. The final `.iso` artifact and its `SHA256SUMS` checksum will be saved in the `output/` directory.

---

## 💻 How to Set Up and Install RatanaOS

### 1. Flash the ISO
Write the generated `.iso` to a USB flash drive using a tool like `dd`, BalenaEtcher, Ventoy, or Rufus.
```bash
sudo dd if=output/RatanaOS-Standard.iso of=/dev/sdX bs=4M status=progress
```
*(Replace `/dev/sdX` with your actual USB drive identifier).*

### 2. Boot the Live System
Insert the USB drive into your target machine and boot from it (Supports both BIOS and UEFI). You will be greeted by the RatanaOS Live Desktop.

### 3. Run the RatanaOS Installer
From the Live Desktop, launch the **RatanaOS Installer** application and follow the 13-step wizard:
1. **Welcome**: Start the installation.
2. **Language / Keyboard**: Set your regional preferences.
3. **Network**: Connect to Wi-Fi or Ethernet for updates.
4. **Timezone**: Select your local timezone.
5. **Disk Selection**: Choose Automatic (Erase Disk) or Manual Partitioning. You can check the boxes for **Btrfs** and **LUKS Disk Encryption**.
6. **User Creation**: Create your administrator account.
7. **Avatar Upload**: Set a custom profile picture.
8. **Desktop Selection**: Choose between KDE Plasma, GNOME, XFCE, or LXQt.
9. **Edition Selection**: Choose between Lite, Standard, Developer, or Cyber base packages.
10. **Package Groups**: Select the optional security or developer toolkits you want to install.
11. **Installation**: Wait for the files to copy and the bootloader to install.
12. **Reboot**: Remove your USB drive and reboot into your new RatanaOS v13 Ultimate system!

---

## 📚 Documentation
For more detailed information, please refer to the files in our `docs/` directory:
- [BUILD.md](docs/BUILD.md) - In-depth build pipeline documentation.
- [INSTALL.md](docs/INSTALL.md) - Detailed installation instructions.
- [USER_GUIDE.md](docs/USER_GUIDE.md) - Guide to using the RatanaOS desktop and applications.
- [SECURITY_GUIDE.md](docs/SECURITY_GUIDE.md) - Overview of the system hardening and cyber tools.
- [DEVELOPER_GUIDE.md](docs/DEVELOPER_GUIDE.md) - Instructions for developing and compiling applications within RatanaOS.

---

## 🤝 Contributing
Please remember that we follow a strict Git workflow. **Never push directly to main.** 
All changes must be made on feature branches following the `Inspect → Implement → Test → Document → Commit → Push → Report` methodology.
