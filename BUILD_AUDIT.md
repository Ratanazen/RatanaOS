# RatanaOS Build Inspection Audit (BUILD_AUDIT.md)

**Date**: 2026-07-20  
**Inspector**: Lead Debian Live Build Engineer  
**Status**: Comprehensive Audit Completed  

---

## Executive Summary

A complete inspection of the build system (`auto/`, `config/`, `builder/`, `scripts/`, `packages/`, `output/`, `logs/`, and `Makefile`) was conducted. The primary root cause for boot failure in generated ISOs is that **kernel (`vmlinuz`) and initrd (`initrd.img`) were faked using 10MB zero-filled dummy files** because `config/chroot` disabled Linux kernel installation (`LB_LINUX_PACKAGES="none"`) and package installation failed due to APT package collisions between `grub-pc` and `grub-efi-amd64`.

---

## Audit Findings by Component

### 1. `auto/` Directory
- **Status**: ❌ **CRITICAL ISSUE** (Empty Directory)
- **Defects**: `auto/` was completely empty. Standard live-build scripts (`auto/config`, `auto/build`, `auto/clean`) were missing.
- **Impact**: `lb config` and `lb build` invocations fell back to unconfigured defaults or manual flags.

### 2. `config/` Directory
- **`config/chroot`**:
  - `LB_LINUX_PACKAGES="none"`: ❌ **CRITICAL**. Instructed live-build NOT to install any Linux kernel package into the chroot filesystem.
  - `LB_UNION_FILESYSTEM="aufs"`: ⚠️ Deprecated union filesystem format for Debian Bookworm (Linux 6.x/7.x requires `overlay`).
- **`config/binary`**:
  - `LB_BOOTLOADER="syslinux"`: ❌ Inconsistent with GRUB2 hybrid EFI/BIOS boot layout.
  - `LB_DEBIAN_INSTALLER_DISTRIBUTION="trixie"`: ⚠️ Mismatched target distribution (`bookworm` vs `trixie`).
  - `LB_BOOTAPPEND_LIVE`: Included `findiso=${iso_path}` which broke standard live boot.
- **`config/common`**:
  - `LB_INITSYSTEM="sysvinit"`: ❌ Incompatible with systemd init pipeline in RatanaOS.
- **`config/package-lists/`**:
  - Package lists specified both `grub-pc` and `grub-efi-amd64`. In Debian APT, these two meta-packages conflict during installation.

### 3. `builder/` Directory
- **`builder/build-iso.sh`**:
  - Contained fallback logic (`stage_kernel`) that generated 10MB zero-filled dummy files with python bzImage header stubs when real `vmlinuz` was missing.
  - Silent failure fallbacks hid underlying live-build compilation failures.
- **`builder/build-ratanaos.sh`**:
  - Hardcoded package lists that duplicated APT conflicts.

### 4. `scripts/` Directory
- **Mock/Stub Scripts**:
  - `check-build-deps.sh` (49 bytes): Stub script (`echo "Dependencies checked."`).
  - `configure-live-build.sh` (50 bytes): Stub script (`echo "Live build configured."`).
  - `qemu-smoke-test.sh` (48 bytes): Stub script (`echo "Smoke test complete."`).
  - `verify-installer.sh` (47 bytes): Stub script (`echo "Installer verified."`).

### 5. `packages/` Directory
- **`packages/ratanaos.list.chroot`**: Outdated 53-byte stub file duplicating `config/package-lists/ratana.list.chroot`.

### 6. `output/` and `logs/`
- **`output/build.log`**: Recorded APT failure: `grub-efi-amd64 : Conflicts: grub-pc`.
- **`logs/`**: Showed recurring build fallbacks into zero-padded fake ISO images.

### 7. `Makefile`
- Missing automated execution of `auto/` workflow and comprehensive ISO structural validation.

---

## Action Plan for Repairs

1. Create `auto/config`, `auto/build`, `auto/clean` scripts.
2. Correct `config/chroot`, `config/binary`, `config/common`, and `config/bootstrap` settings (`bookworm`, `overlay`, `systemd`, `linux-image`).
3. Resolve `grub-pc` / `grub-efi-amd64` APT conflicts by using binary support packages `grub-pc-bin`, `grub-efi-amd64-bin`, `grub-efi-amd64-signed`.
4. Ensure real Linux kernel (`/boot/vmlinuz-*`) and initrd (`/boot/initrd.img-*`) are generated and copied to `/live/vmlinuz` and `/live/initrd.img`.
5. Remove mock zero-filled fallback generators from `build-iso.sh`.
6. Implement functional script logic in `scripts/`.
