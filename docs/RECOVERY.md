# RatanaOS Recovery Guide (v18)

RatanaOS provides a robust suite of recovery tools designed to help you troubleshoot and resolve system issues. This guide covers how to access and use the **Ratana Recovery** environment.

## Accessing Recovery Mode

To access Recovery Mode:
1. Reboot your computer.
2. When the GRUB bootloader menu appears, select **"Start RatanaOS (Recovery Mode)"**.
3. The system will boot directly into the Ratana Recovery application.

## Recovery Tools

The Ratana Recovery environment provides the following tools:

### 1. Btrfs Snapshots
If your system supports Btrfs, you can easily roll back to a previous known good state. This is highly recommended before running more destructive repair commands.
- **Roll Back**: Select a snapshot from the list and click "Roll Back to Selected Snapshot". The system will automatically revert to this state.

### 2. Boot Repair
If the bootloader is broken or the system cannot find the OS:
- **Reinstall GRUB**: Reinstalls the GRUB bootloader for both UEFI and BIOS systems.
- **Regenerate initramfs**: Rebuilds the initial ramdisk, which is useful if kernel modules or boot configurations are corrupt.
- **Check EFI Variables**: Diagnoses issues with UEFI boot entries.

### 3. Diagnostics
A suite of hardware and filesystem tests:
- Checks CPU temperature and health.
- Runs memory tests to detect faulty RAM.
- Checks disk SMART status for potential drive failures.
- **Run Full Filesystem Check (fsck)**: Scans and repairs filesystem errors.

### 4. System Logs
A live viewer for system and kernel logs (`dmesg` and `journalctl`). Useful for identifying boot errors, missing drivers, or hardware initialization issues.

### 5. Emergency Shell
Advanced users can launch a direct command-line interface:
- **Launch /bin/bash (Root)**: Opens a root terminal with full privileges.
- **Mount System Read/Write**: Mounts the main filesystem so you can edit configuration files.
- **Chroot into Installed OS**: Enters the installed system's environment to run commands as if you had booted normally.

### 6. Crash Reports
If the system or applications crashed prior to recovery, you can export these diagnostic files (`/var/crash/`) to a USB drive for later analysis or to share with support teams.

## Emergency Contacts
If these tools do not resolve your issue, please seek help at [https://ratanaos.local/support](https://ratanaos.local/support).
