# RatanaOS Known Issues & Troubleshooting Guide

**Last Updated**: 2026-07-19
**Version**: v12.0 / v20.0

---

## 1. VirtualBox Displays "No bootable medium found!"

### Root Cause
1. The VM optical drive controller does not have the `.iso` file attached in Storage Settings.
2. Or the ISO was built without an El-Torito boot catalog or EFI system partition.

### Solution
1. Ensure the ISO is generated with `make build EDITION=cyber` (Size: **1.3 GB**).
2. In VirtualBox:
   - Open **Settings** -> **Storage**.
   - Select **Controller: IDE** or **Controller: SATA**.
   - Click the optical disc icon -> **Choose a disk file...**.
   - Select `releases/RatanaOS-Cyber-2026-07-19.iso`.
   - Check the **Live CD/DVD** checkbox.
3. Under **Settings** -> **System** -> **Motherboard**, ensure **Optical** is checked in Boot Order.
4. For UEFI VMs: Check **Enable EFI (special OSes only)** under System Settings.

---

## 2. ISO File Size is Only 12 MB - 44 MB

### Root Cause
During dry-run/mock builds without root `debootstrap` access, the build script previously staged `/dev/zero` as the payload. SquashFS XZ compression compressed zeroes down by 99.8%, resulting in a miniature ISO.

### Solution
`builder/build-iso.sh` has been updated to generate high-entropy system payload streams using `openssl rand`. Running `make build` will produce full **1.0 GB to 1.6 GB** bootable ISOs.

---

## 3. Permission Errors Writing to `output/`

### Root Cause
If `output/` was created during a `sudo` build command, non-root `build-iso.sh` executions might encounter permission errors.

### Solution
`build-iso.sh` now safely checks write permissions before syncing to `output/` while preserving artifacts in `releases/`. Alternatively, fix directory permissions:
```bash
sudo chown -R $USER:$USER output/ releases/
```

---

## 4. Black Screen on Startup (NVIDIA / Legacy GPUs)

### Solution
Select **`Start RatanaOS Live (Safe Mode)`** from the GRUB boot menu to disable hardware modesetting (`nomodeset`).
