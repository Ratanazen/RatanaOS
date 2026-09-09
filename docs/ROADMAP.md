# 🗺️ RatanaOS Development Roadmap

RatanaOS is an independent, standalone 64-bit desktop operating system developed for x86-64 hardware with host builds on Debian Linux.

---

## 📌 Phase Overview & Status

```text
 PHASE 1: Audit & Baseline Build ────────────────────► [COMPLETED]
 PHASE 2: Kernel Core, MM, Interrupts & Timer ──────► [COMPLETED]
 PHASE 3: Device Drivers (PS/2, PCI, ATA, e1000) ───► [COMPLETED]
 PHASE 4: VFS, DevFS, ProcFS & Ext2 Filesystem ──────► [IN PROGRESS]
 PHASE 5: Graphics Engine, Fonts & UI Scaling ──────► [COMPLETED]
 PHASE 6: Theme Engine & Icon Integration ──────────► [COMPLETED]
 PHASE 7: Window Manager, Desktop, Dock & Menu Bar ─► [COMPLETED]
 PHASE 8: System Settings & Persistent Config ──────► [COMPLETED]
 PHASE 9: Shell, SysInfo & System Services ─────────► [COMPLETED]
 PHASE 10: Built-in Applications & Desktop Apps ────► [IN PROGRESS]
 PHASE 11: Package Architecture (.rpk & Debian) ────► [COMPLETED]
 PHASE 12: Performance, Security & Hardening ────────► [IN PROGRESS]
```

---

## 📋 Phase Details

### Phase 1: Repository Audit & Baseline Build
- Audit repository structure, build setup, Makefile, linker scripts, kernel entry points.
- Verify 64-bit ELF binary layout and Multiboot compliance.
- Establish core documentation (`docs/ARCHITECTURE.md`, `docs/BUILD.md`, `docs/BASELINE.md`, `docs/ROADMAP.md`).

### Phase 2: Kernel Core & Memory Management
- Clean modular kernel initialization (`kernel.c`, `gdt.c`, `idt.c`, `isr.c`).
- Physical memory manager (Bitmap page frame allocator).
- Virtual memory manager (4-level PML4 paging, identity mapping up to 4GB).
- Kernel heap allocator (`kmalloc`, `kfree`).
- Preemptive and cooperative round-robin process scheduler.

### Phase 3: Hardware Drivers
- PS/2 Keyboard and PS/2 Mouse input drivers with packet handling.
- PCI Bus Enumeration driver.
- ATA PIO disk storage driver.
- Intel e1000 Gigabit Ethernet NIC driver.
- RTC driver and programmable interval timer (PIT / 8254).

### Phase 4: Virtual File System & Storage
- VFS polymorphic device interface (`open`, `close`, `read`, `write`, `stat`, `readdir`).
- Pseudo filesystems: DevFS (`/dev/null`, `/dev/zero`, `/dev/console`, `/dev/tty`) & ProcFS (`/proc/cpuinfo`, `/proc/meminfo`, `/proc/uptime`, `/proc/version`).
- Ext2 filesystem reader/writer.
- Initramfs embedded RAM disk image.

### Phase 5: Graphics Engine, Font Renderer & UI Scaling
- 32-bit ARGB software graphics renderer (`gfx.c`) with double buffering and clipping.
- Dynamic bitmap font system with multiple font sizes.
- Scale-aware UI layout system (`ui_scale_set()`, `ui_scale_px()`).

### Phase 6: Theme Engine & Icon System
- Global design token system (`src/kernel/theme.c`).
- Icon Engine supporting WhiteSur, MacTahoe, and Vector fallback themes.
- Dark mode, Light mode, and Auto theme switching.

### Phase 7: Desktop Environment & Window Manager
- Translucent top menu bar with system indicators (Clock, Battery, Network, Sound).
- Floating desktop Dock with active application indicators and magnification effects.
- Window Manager with title bars, traffic lights (🔴 🟡 🟢), dragging, and focus z-order.

### Phase 8: System Settings & Config Persistence
- Tabbed System Settings application (Appearance, Desktop, System, Security, About).
- Persistent configuration storage (`/etc/ratana/settings.conf`).

### Phase 9: Shell & Core Services
- Interactive shell (`/bin/sh`) with builtins (`cd`, `pwd`, `ls`, `cat`, `echo`, `ps`, `mem`, `uname`, `theme`, `icons`).
- System services daemon suite (`ratanaos-health`, `ratanaos-firstboot`).

### Phase 10: Built-in Desktop Applications
- Terminal emulator (`/bin/sh`).
- System Settings app (`gui.c` settings dialog).
- File Manager, System Monitor, Calculator, Text Editor.

### Phase 11: Package & Software Distribution Architecture
- Native `.rpk` package manager (`ratapkg`).
- Debian package importer and compatibility validation (`debimport`).

### Phase 12: Performance, Hardening & Security
- Kernel panic diagnostics with register dump (RIP, RSP, CR2, Exception code).
- System security hardening tools (`ratanaos-harden`, `ratanaos-forensics`, `ratanaos-recon`).
- Full automated test suite (`make test-all`).
