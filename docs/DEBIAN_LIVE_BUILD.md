# 🏗️ Debian Live-Build Integration Architecture (v4.0)

This document details the integration of the official **Debian Live-Build (`live-build`)** toolchain into the **RatanaOS** build system for generating and packaging **Debian GNU/Linux 13 "Trixie" (amd64)** userland environments.

---

## 📌 Architecture & Build Pipeline

```text
               Debian live-build / debootstrap
                              │
                              ▼
               Debian 13 "Trixie" (amd64) packages
                              │
                              ▼
               Debian Root Filesystem (build/debian-rootfs/)
                              │
                              ▼ (tools/bake_debian_rootfs.py)
               RatanaOS DebianFS Container (build/debian.img)
                              │
                              ▼ (src/kernel/fs/debianfs.c)
               RatanaOS Virtual File System (VFS)
                              │
                    ┌─────────┴─────────┐
                    ▼                   ▼
            /system/debian/       /mnt/debian/
```

---

## 🔍 Critical Conceptual Distinctions

To prevent architectural confusion, RatanaOS establishes strict boundaries between layers:

1. **Debian Live Image (`.iso`)**:
   - A standalone, bootable live installer image produced by `live-build`.
   - Used exclusively in **Track A (Dual Boot)** for chainloading or loopback booting alongside RatanaOS via GRUB 2.
2. **Debian RootFS (`rootfs`)**:
   - The directory hierarchy of Debian userland binaries (`/bin`, `/etc`, `/lib`, `/usr`) without a Linux kernel.
   - Used in **Track B (Debian RootFS)** to mount into the RatanaOS polymorphic VFS under `/system/debian/`.
3. **RatanaOS Executable Environment**:
   - The native RatanaOS 64-bit kernel, memory manager, scheduler, and **Track C (Linux ABI Layer)**.
   - Provides the system call gates (`LSTAR 0xC0000082` and `int $0x80`), ELF64 loader (`PT_LOAD` and `PT_INTERP`), and libc environment to execute Debian binaries natively.

---

## 🛠️ Toolchain Structure & Usage

The live-build toolchain is organized in `tools/debian/`:

```text
tools/
├── debian/
│   ├── auto/
│   │   └── config                  # Auto-configuration for live-build (Trixie amd64)
│   ├── config/
│   │   └── package-lists/
│   │       └── minimal.list.chroot # Minimal package set (busybox, coreutils, libc6, dash, bash)
│   └── README.md
├── ratana-debian-build.sh           # Main orchestration script
└── bake_debian_rootfs.py           # RatanaOS container packager with traversal validation
```

### Makefile Targets

| Command | Action | Output |
| :--- | :--- | :--- |
| `make debian-live` | Invokes `tools/ratana-debian-build.sh` | Generates RootFS and packages `build/debian.img` |
| `make debian-rootfs` | Re-bakes RootFS into C data array | `src/kernel/fs/debian_data.c` |
| `make iso` | Builds complete bootable ISO | `build/ratanaos.iso` |
| `make iso-with-debian DEBIAN_ISO=...` | Embeds Debian installer ISO | Hybrid dual-boot ISO |
