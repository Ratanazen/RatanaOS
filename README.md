# RatanaOS v5.0 "Phoenix"

[![Build Status](https://github.com/your-username/RatanaOS/actions/workflows/main.yml/badge.svg)](https://github.com/your-username/RatanaOS/actions)
[![Latest Release](https://img.shields.io/github/v/release/your-username/RatanaOS)](https://github.com/your-username/RatanaOS/releases/latest)
[![License: GPL-3.0](https://img.shields.io/badge/License-GPL%203.0-blue.svg)](LICENSE)

> **Replace `your-username` above with your actual GitHub username before publishing.**

---

## 📥 Download

> **[⬇️ Download RatanaOS from the GitHub Releases page](https://github.com/your-username/RatanaOS/releases/latest)**

| Edition | File | Size | Best For |
|---|---|---|---|
| 🪶 Lite | `RatanaOS-Lite.iso` | ≈2 GB | Older hardware, XFCE |
| 🖥️ Standard | `RatanaOS-Standard.iso` | ≈3 GB | Everyday desktop, KDE |
| 🧑‍💻 Developer | `RatanaOS-Developer.iso` | ≈3.5 GB | Programming, containers |
| 🔐 Cyber | `RatanaOS-Cyber.iso` | ≈4 GB | Cybersecurity education |
| 🗄️ Server | `RatanaOS-Server.iso` | ≈800 MB | Headless servers |
| 🍓 ARM64 | `RatanaOS-ARM64.img` | ≈2 GB | Raspberry Pi 4/5, ARM SBCs |

### Verify Your Download
```bash
# Download the checksum file alongside your ISO, then run:
sha256sum -c SHA256SUMS
```

---

# RatanaOS Cyber Edition v1.0

Welcome to **RatanaOS Cyber Edition**, a specialized, Debian-based Linux distribution engineered from the ground up for cybersecurity professionals, software developers, and system administrators. 

RatanaOS provides an original, curated user experience, shipping with a bespoke Qt6-based desktop architecture, hardened security defaults, and a modular ISO build system.

---

## 🌟 Key Features

- **Four Custom Build Profiles**: Choose between `ratana-cyber`, `ratana-developer`, `ratana-workstation`, and `ratana-lite` depending on your hardware and professional needs.
- **Dynamic Installer**: A beautiful Qt6 graphical installer that supports Btrfs, LUKS encryption, and on-the-fly "Package Profile" selections so you can install specific cybersecurity toolkits (e.g., Malware Analysis, Packet Capture) directly from the Live USB.
- **Pre-configured Development Environment**: Out-of-the-box support for GCC, Clang, CMake, Git, Python 3, Rust, Go, Java, Node.js, and Docker.
- **Hardened Security**: Default-deny UFW firewall, enforced AppArmor profiles, and strict Auditd tracking out of the box.
- **Original Qt6 Applications**:
  - **Ratana Terminal**: A GPU-accelerated workspace terminal.
  - **Ratana Software Center**: Unified APT and Flatpak graphical package management.
  - **Ratana Update Manager**: Background daemon for system upgrades and Btrfs snapshot rollbacks.
  - **RatanaAI Assistant**: Built-in AI copilot for shell command explanations and system troubleshooting.

---

## 🛠️ How to Build the ISO

The RatanaOS build system utilizes proven Linux tools (`live-build`, `debootstrap`, `mksquashfs`, `xorriso`) to generate a hybrid UEFI/BIOS bootable Live ISO.

### Prerequisites
You must be running a Debian-based host system with the following packages installed:
```bash
sudo apt update
sudo apt install debootstrap live-build grub-pc-bin grub-efi-amd64-bin xorriso squashfs-tools mtools dosfstools rsync qemu-system-x86 ovmf
```

### Build Instructions
1. Clone the repository and navigate to the root directory.
2. Run the master CI/CD pipeline script to automatically test and build the default profile:
   ```bash
   ./tests/ci_pipeline.sh
   ```
3. **Or**, run the ISO builder manually and specify your target profile (`ratana-cyber`, `ratana-developer`, `ratana-workstation`, `ratana-lite`):
   ```bash
   ./builder/build-iso.sh ratana-cyber
   ```
4. The build process will bootstrap a Debian base, apply the RatanaOS overlays, generate a SquashFS filesystem, and package it into an ISO.
5. The final `.iso` artifact and its `SHA256SUMS` checksum will be saved in the `output/` directory.

---

## 💻 How to Set Up and Install RatanaOS

### 1. Flash the ISO
Write the generated `RatanaOS-amd64.iso` to a USB flash drive using a tool like `dd`, BalenaEtcher, or Rufus.
```bash
sudo dd if=output/RatanaOS-amd64.iso of=/dev/sdX bs=4M status=progress
```
*(Replace `/dev/sdX` with your actual USB drive identifier).*

### 2. Boot the Live System
Insert the USB drive into your target machine and boot from it (Supports both BIOS and UEFI). You will be greeted by the RatanaOS Live Desktop.

### 3. Run the RatanaOS Installer
From the Live Desktop, launch the **RatanaOS Installer** application and follow the wizard:
1. **Welcome**: Start the installation.
2. **Language / Keyboard / Timezone**: Set your regional preferences.
3. **Disk Selection**: Choose Automatic (Erase Disk) or Manual Partitioning. You can check the boxes for **Btrfs** and **LUKS Disk Encryption**.
4. **User Creation**: Create your administrator account and set a hostname.
5. **Package Profile**: Select the optional security toolkits you want to install (e.g., Digital Forensics, Reverse Engineering).
6. **Installation**: Wait for the files to copy and the bootloader to install.
7. **Reboot**: Remove your USB drive and reboot into your new RatanaOS system!

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
