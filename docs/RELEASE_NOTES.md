# RatanaOS Release Notes

---

## v20.0.0 — Productivity & Customization

### What's New in v20.0
- **Dynamic Real-Time Theming**: Integrated `RatanaOS::Customization` API with live CSS stylesheet reloads across Qt6 applications without reboot.
- **AMOLED & Light Profiles**: Added high-contrast AMOLED and Light theme presets.
- **Fixed Compilation & UI Headers**: Applied clean compilation fixes for `ratana_ui` and `appearance-center`.

---

## v19.0.0 — Hardware & Welcome Experience

### What's New in v19.0
- **Welcome Center Rewrite**: Completely overhauled first-boot setup wizard.
- **Hardware & Driver Scanning**: Integrated GPU (Nouveau vs NVIDIA), Wi-Fi firmware, Bluetooth, and network printer detection.
- **Centralized CSS Styling**: Extracted all Qt default styles into unified `/assets/ratanaos.css`.

---

## v18.0.0 — Recovery & Reliability

### What's New in v18.0
- **Recovery Mode**: Boot entry booting directly into `ratana-recovery`.
- **Emergency Rescue Tools**: Boot repair, log viewer, crash dumps, and hardware diagnostics.
- **Documentation**: Added `docs/RECOVERY.md`.

---

## v12.0 — Universal Live USB & GUI Installer

### What's New in v12.0
- **Live USB ISO** (`RatanaOS-Live.iso`) — single bootable image for UEFI + BIOS systems.
- **Redesigned GUI Installer** — 11-screen Qt6 wizard with Network detection, Personalization, animated progress.
- **Cyber Edition tools now mandatory** — nmap, nikto, Wireshark, aircrack-ng, hashcat, Hydra, Ghidra always installed.
- **Old ISO cleanup** — build pipeline removes stale placeholder ISOs automatically.
- **Dedicated Live USB builder** (`builder/build-live-usb.sh`).
- **5 new test reports** — LIVE_USB_REPORT, INSTALL_REPORT, VM_REPORT, QEMU_REPORT, VIRTUALBOX_REPORT, VMWARE_REPORT.

---

## v11.1 — Customization & Personalization

# RatanaOS v10.1 Release Notes

## Overview
RatanaOS v10.1 is the latest production release focusing on extreme stability, automated testing pipelines, and expanded architecture support.

## Supported Editions
1. **RatanaOS Lite** (XFCE) - Targeting ≈2GB for older hardware.
2. **RatanaOS Standard** (KDE Plasma) - Everyday desktop usage.
3. **RatanaOS Developer** (KDE Plasma) - Full dev toolchain included.
4. **RatanaOS Cyber** (KDE Plasma) - Cybersecurity toolkits.

## Testing & Quality Assurance
- Automated QEMU, VirtualBox, and VMware boot tests.
- Extensive CI/CD validation before any merge.
- Fully validated hardware detection for CPU, RAM, Disk, GPU, Audio, Bluetooth, WiFi, and Ethernet.
