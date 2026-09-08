# RatanaOS on Oracle VirtualBox: Setup & Execution Guide

Follow this guide to run **RatanaOS 64-bit (macOS Sequoia Edition)** in **VirtualBox** with 100% error-free performance.

---

## 1. Create a New Virtual Machine

1. Open VirtualBox and click **New** (`Ctrl + N`).
2. **Name and Operating System**:
   - **Name**: `RatanaOS 64-bit`
   - **Type**: `Other`
   - **Version**: `Other/Unknown (64-bit)`
3. **Hardware Configuration**:
   - **Base Memory (RAM)**: `512 MB` (or `1024 MB`)
   - **Processors**: `1 CPU` or `2 CPUs`
   - **Enable EFI**: *Optional (Disabled by default for standard BIOS boot)*
4. **Hard Disk**:
   - Select **Do not add a virtual hard disk** (or create a small 2GB VDI).

---

## 2. Recommended VM Settings

Click on your VM and open **Settings** (`Ctrl + S`):

### 1. System:
- **Motherboard**:
  - Chipset: `PIIX3` (or `ICH9`)
  - Pointing Device: `PS/2 Mouse`
  - Enable I/O APIC: `Checked`
- **Processor**:
  - Enable PAE/NX: `Checked`

### 2. Display:
- **Video Memory**: `32 MB` or `64 MB`
- **Graphics Controller**: `VBoxVGA` or `VMSVGA` (both supported)
- **3D Acceleration**: `Disabled`

### 3. Storage:
- Under **Storage Devices**, select the empty optical drive.
- Click the disc icon on the right and select **Choose a disk file...**
- Browse and attach: `build/ratanaos.iso`.

### 4. Audio & Network:
- Audio Controller: `ICH AC97` or `SoundBlaster 16`.
- Network: `Intel PRO/1000 MT Desktop` (NAT).

---

## 3. Starting RatanaOS in VirtualBox

1. Click **Start** (`Normal Start`).
2. The GRUB bootloader menu will appear automatically:
   - Select **`RatanaOS 64-bit (macOS Sequoia Edition)`** (default).
3. The system will boot immediately into the 64-bit kernel.
4. Type `gui` in the shell to enter the **macOS Sequoia Desktop Environment**:
   ```bash
   ratana@os-x86_64 > gui
   ```
5. Use your mouse to interact with windows, the Top Menu Bar, and the Floating Dock!
