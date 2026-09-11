# RatanaOS Installation Guide (Real PC & Virtual Machines)

This guide covers installing **RatanaOS 2026 (macOS Sequoia Edition)** onto a real computer (NVMe/SATA SSD or HDD) or virtual machine using either the Graphical Installer (Calamares) or the Terminal Installer (`ratana-installer`).

---

## 1. System Requirements

| Component | Minimum | Recommended |
| :--- | :--- | :--- |
| **Processor** | 64-bit x86_64 Dual Core | 64-bit Quad Core (Intel Core i5/i7 or AMD Ryzen) |
| **Memory (RAM)** | 2.0 GB | 8.0 GB+ (enables *Beautiful* blur & animation profile) |
| **Storage** | 20 GB free disk space | 64 GB+ NVMe SSD |
| **Graphics** | Standard VGA / Intel HD | AMD Radeon / Intel Iris Xe / NVIDIA (with modesetting) |
| **Firmware** | UEFI or BIOS/Legacy | UEFI with Secure Boot (supported via Debian signed shim) |

---

## 2. Booting the Live USB

1. Flash the hybrid ISO to a USB flash drive (>= 8GB) using `dd` or Rufus / balenaEtcher:
   ```bash
   sudo dd if=ratanaos-live-amd64.hybrid.iso of=/dev/sdX bs=4M status=progress oflag=sync
   ```
2. Insert the USB drive into your PC and boot into the Boot Selection menu (F12, F11, F8, or Option on Mac).
3. Select **RatanaOS 2026 Live (macOS Sequoia Edition)** from the bootloader menu.

---

## 3. Method A: Graphical Installation (Calamares)

1. Once the live desktop loads, click the **Install RatanaOS** icon on the desktop or dock.
2. Select your Language and Timezone.
3. Choose your keyboard layout.
4. **Partitions Screen**:
   - **Erase disk**: Automatically partitions the disk with a 512MB FAT32 EFI partition and ext4 root partition.
   - **Manual partitioning**: Allows custom root, home, and swap layouts.
5. Enter your User Name and Password (default: `ratana`).
6. Click **Install Now** and wait for the installer to finish.
7. Click **Restart Now** and remove the Live USB medium when prompted.

---

## 4. Method B: Terminal Installation (`ratana-installer`)

If installing on headless hardware, serial console, or if you prefer a keyboard-driven TUI flow:

1. Open a terminal (Super+Return or Kitty) or switch to TTY2 (`Ctrl+Alt+F2`).
2. Execute:
   ```bash
   sudo ratana-installer
   ```
3. The installer scans all block devices (strictly filtering out the live installer USB).
4. Select your target disk number.
5. Review the disk details (Model, Capacity, Bus) and type `YES` to confirm.
6. The installer automatically executes:
   - Partitioning (GPT for UEFI, MBR for BIOS)
   - Filesystem formatting (FAT32 for ESP, ext4 for `/`)
   - System synchronization & live-boot package purge
   - User creation and sudo privileges
   - NetworkManager configuration
   - GRUB bootloader installation (`x86_64-efi` or `i386-pc`)
   - Persistent `fstab` generation by UUID
7. Reboot your machine:
   ```bash
   sudo reboot
   ```
