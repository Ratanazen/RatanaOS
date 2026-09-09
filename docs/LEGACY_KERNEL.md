# 📖 RatanaOS Custom Kernel & Architecture Reference (Legacy Core)

This document preserves the comprehensive architecture and implementation details of the custom 64-bit microkernel / hybrid kernel developed for RatanaOS. All source files in `src/`, `build/ratanaos.bin`, and associated unit tests remain fully intact in the repository.

---

## 🏛️ Kernel Subsystems & Source Map

```text
src/
├── boot/
│   ├── boot.asm             # 32-bit Multiboot entry point, GDT, Paging setup
│   └── long_mode.asm        # Transition to 64-bit Long Mode, kernel stack setup
├── kernel/
│   ├── main.c               # Kernel initialization sequence, ACPI, GDT/IDT
│   ├── memory/
│   │   ├── pmm.c            # Physical Memory Manager (Bitmap allocator)
│   │   └── vmm.c            # Virtual Memory Manager (4-level PML4 paging)
│   ├── process/
│   │   ├── scheduler.c      # Preemptive round-robin task scheduler
│   │   ├── process.c        # Process Control Blocks (PCB) & context switching
│   │   └── elf.c            # 64-bit ELF binary loader (PT_LOAD, PT_INTERP)
│   ├── syscall/
│   │   └── syscall.c        # Linux x86-64 ABI & native syscall gate
│   ├── fs/
│   │   ├── vfs.c            # Virtual File System abstraction layer
│   │   ├── ramfs.c          # In-memory RAM filesystem
│   │   ├── devfs.c          # Device filesystem (/dev/fb0, /dev/tty)
│   │   └── debianfs.c       # Container filesystem mounted at /system/debian
│   ├── gui/
│   │   ├── compositor.c     # 32-bit ARGB double-buffered window compositor
│   │   ├── window.c         # Window management with macOS traffic lights
│   │   ├── dock.c           # Centered application dock with icon magnification
│   │   └── theme.c          # macOS Sequoia dark/light theme engine
│   └── drivers/
│       ├── framebuffer.c    # VESA / GOP linear framebuffer driver
│       ├── keyboard.c       # PS/2 keyboard controller
│       ├── mouse.c          # PS/2 mouse driver with packet parsing
│       ├── serial.c         # 16550 UART COM1 serial logging
│       ├── timer.c          # PIT (Programmable Interval Timer) & RTC clock
│       └── icons_assets.c   # 32-bit ARGB icon assets for WhiteSur & MacTahoe
└── include/                 # Subsystem header files & API definitions
```

---

## ⚙️ Core Technical Highlights

### 1. Boot & Paging
- Compliant with Multiboot specification v1.
- Initial identity mapping of low 20MB with 2MB huge pages.
- Dynamic 4-level PML4 paging with table shattering down to 4KB pages for fine-grained Ring 3 user memory isolation.

### 2. Preemptive Task Scheduling & Ring 3
- Preemptive scheduling driven by PIT timer interrupts at 100 Hz.
- Task context switching saving general purpose registers, segment registers, and CPU flags.
- Ring 3 user execution with separate user stack and TSS privilege stacks.

### 3. Linux x86-64 ABI Compatibility Gate
- Fast syscall invocation via `syscall` / `sysretq` instructions using MSR LSTAR (`0xC0000082`).
- Support for key Linux syscalls: `read` (0), `write` (1), `open` (2), `close` (3), `fstat` (5), `mmap` (9), `brk` (12), `arch_prctl` (158, `ARCH_SET_FS`), `set_tid_address` (218), `exit_group` (231).
- Auxiliary vector (`auxv`) stack preparation for dynamic linkers (`ld-linux-x86-64.so.2`).

### 4. Native Double-Buffered Framebuffer GUI
- Linear framebuffer rendering with 32-bit ARGB color space.
- Window management with draggable title bars, drop shadows, and macOS-style traffic light window controls (`#FF5F56`, `#FFBD2E`, `#27C93F`).
- Built-in WhiteSur and MacTahoe raster icon rendering engine.

---

## 🧪 Building & Running the Custom Kernel

To build and run the freestanding custom kernel without affecting the Debian Live distribution:

```bash
# Compile custom kernel binary (build/ratanaos.bin)
make

# Run all 19 subsystem unit tests
make test-all

# Boot custom kernel in QEMU
make run
```
