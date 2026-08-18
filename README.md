# RatanaOS (2026 Full-Feature Edition)

![Year](https://img.shields.io/badge/Edition-2026_Full--Feature-blueviolet)
![Architecture](https://img.shields.io/badge/Architecture-x86_%7C_i686-blue)
![Language](https://img.shields.io/badge/Language-C23_%2F_NASM-orange)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen)
![License](https://img.shields.io/badge/License-MIT-green)

**RatanaOS 2026** is a modular, high-performance 32-bit x86 operating system kernel with full hardware drivers, memory management, peripheral subsystem support, and an interactive CLI shell with games and utilities.

---

## Key Subsystems & Architecture

```
                                  +---------------------------------------+
                                  |            RatanaSH 2026              |
                                  | (fetch, date, pci, mem, matrix, snake)|
                                  +-------------------+-------------------+
                                                      |
                   +----------------------------------+----------------------------------+
                   |                                  |                                  |
            +------v-------+                   +------v-------+                   +------v-------+
            |  VGA Driver  |                   |  PS/2 Driver |                   | CMOS / RTC   |
            | (80x25 Color)|                   |  (Keyboard)  |                   | (2026 Clock) |
            +------+-------+                   +------+-------+                   +------+-------+
                   |                                  |                                  |
            +------v-------+                   +------v-------+                   +------v-------+
            | PC Speaker   |                   | Serial COM1  |                   | PCI Bus      |
            | (PIT Sound)  |                   | (Debug Logs) |                   | (Enumerator) |
            +------+-------+                   +------+-------+                   +------+-------+
                   |                                  |                                  |
+------------------v----------------------------------v----------------------------------v------------------+
|                                              Kernel Core                                                  |
|  +--------------------+  +--------------------+  +--------------------+  +--------------------+           |
|  | GDT (Segmentation) |  | IDT & ISR (0-31)   |  | PIC 8259 & PIT 100 |  | CPUID Inspector    |           |
|  +--------------------+  +--------------------+  +--------------------+  +--------------------+           |
|  +--------------------------------------------+  +--------------------------------------------+           |
|  | PMM (128MB Bitmap Allocator)               |  | Kernel Heap (kmalloc, kcalloc, kfree)      |           |
|  +--------------------------------------------+  +--------------------------------------------+           |
+-----------------------------------------------------+-----------------------------------------------------+
                                                      |
                                      +---------------v----------------+
                                      |    Multiboot Bootloader (ASM)  |
                                      +--------------------------------+
```

---

## Features

### 1. Memory Management
- **Physical Memory Manager (PMM)**: Bitmap page frame allocator managing 128MB of physical RAM in 4KB frames.
- **Dynamic Kernel Heap**: `kmalloc()`, `kcalloc()`, `krealloc()`, and `kfree()` with block splitting and automatic coalescing.

### 2. Hardware Subsystems & Drivers
- **CMOS Real-Time Clock (RTC)**: Accurate date and time synchronization for 2026 timestamps.
- **Serial COM1 (`0x3F8`)**: Early kernel debugging and headless logging.
- **PC Speaker Driver**: Tone generator and frequency synthesis using PIT channel 2 and port `0x61`.
- **PCI Bus Scanner**: Enumerate PCI buses 0-255, identifying vendor IDs, device IDs, and device classes (VGA, Storage, Network, Bridge).
- **CPUID Subsystem**: Query CPU vendor, brand string, family/model, and hardware feature flags (FPU, MMX, SSE, SSE2, SSE3, HTT).
- **VGA Color Display**: 80x25 text mode with 16 foreground/background colors, hardware cursor control, backspace, and smooth scrolling.
- **PS/2 Keyboard Driver**: Interrupt-driven (IRQ1) with US-QWERTY key mapping, Shift, and Caps Lock support.

### 3. Interactive Shell Commands (`RatanaSH 2026`)

| Command | Description |
| :--- | :--- |
| `fetch` | System overview with custom RatanaOS ASCII logo & hardware stats |
| `date` / `time` | Query hardware CMOS Real-Time Clock (2026) |
| `mem` / `free` | Display Physical Memory & Kernel Heap stats |
| `pci` | Scan and display all connected PCI devices |
| `cpuid` | Inspect CPU vendor, brand name, and feature flags |
| `calc <a> <op> <b>` | Built-in arithmetic calculator (`+`, `-`, `*`, `/`, `%`) |
| `beep [freq] [ms]` | Play sound tone through PC speaker |
| `theme <name>` | Apply preset themes (`arch`, `cyber2026`, `matrix`, `ocean`, `amber`) |
| `matrix` | Digital rain screensaver in VGA text mode |
| `snake` | Playable interactive Snake arcade game inside the kernel |
| `color <fg> [bg]` | Set custom foreground and background colors (0-15) |
| `uptime` | System uptime in seconds and timer ticks |
| `clear` | Clear the screen |
| `echo <text>` | Print text to console |
| `about` | Developer info and specifications |
| `reboot` | Reboot machine via keyboard controller reset |
| `halt` | Halt CPU |

---

## Building & Testing

### 1. Requirements (Garuda / Arch Linux)
```bash
sudo pacman -S gcc nasm qemu-system-x86 grub
```

### 2. Build Kernel
```bash
make clean && make
```

### 3. Run in QEMU (Direct Kernel Boot + Serial Logging)
```bash
make run
```

### 4. Build & Run Bootable ISO
```bash
make iso
make run-iso
```
