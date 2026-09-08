# RatanaOS on VMware Workstation / Player / Fusion Guide

Follow this guide to run **RatanaOS 64-bit (macOS Sequoia Edition)** in **VMware Workstation, Player, or Fusion**.

---

## 1. Create a New Virtual Machine

1. Open VMware and click **Create a New Virtual Machine**.
2. Select **Custom (advanced)** or **Typical**.
3. **Guest Operating System Installation**:
   - Choose **Installer disc image file (iso)**.
   - Browse and select: `build/ratanaos.iso`.
4. **Guest Operating System**:
   - **OS**: `Other`
   - **Version**: `Other 64-bit`
5. **Name and Location**:
   - Virtual Machine Name: `RatanaOS 64-bit`
6. **Hardware Allocation**:
   - **Memory**: `512 MB` (or `1024 MB`)
   - **Processors**: `1 Core`
   - **Network Connection**: `Use bridged networking` or `NAT`
   - **Display**: Use standard graphics acceleration (32MB+).

---

## 2. Booting RatanaOS in VMware

1. Click **Power on this virtual machine**.
2. When the GRUB menu appears, press **Enter** on **`RatanaOS 64-bit (macOS Sequoia Edition)`**.
3. The 64-bit Long Mode kernel will initialize with full VBE framebuffer support.
4. In the shell, launch the desktop by typing:
   ```bash
   ratana@os-x86_64 > gui
   ```
5. Press **`ESC`** or choose **`Exit to CLI`** from the  Apple Menu to return to the shell.
