# RatanaOS 64-bit (macOS Sequoia Edition)

<div align="center">

![Year](https://img.shields.io/badge/Release-2026_Sequoia_Edition-blueviolet?style=for-the-badge)
![Architecture](https://img.shields.io/badge/Architecture-x86__64_%7C_AMD64_Long_Mode-blue?style=for-the-badge)
![Graphics](https://img.shields.io/badge/GUI-macOS_Aqua_%2F_VBE_32--bit-brightgreen?style=for-the-badge)
![Language](https://img.shields.io/badge/Language-C23_%2F_NASM_64-orange?style=for-the-badge)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

**A native 64-bit x86_64 operating system kernel featuring a complete macOS Sequoia-style Graphical Desktop Environment, 4-Level Paging (PML4), 64-bit Interrupt Handling, Dynamic Memory Management, Hardware Drivers, and Interactive Desktop Applications.**

</div>

---

## Table of Contents
- [Architecture Overview](#architecture-overview)
- [macOS Sequoia Desktop Environment](#macos-sequoia-desktop-environment)
- [Core Subsystems & Hardware Drivers](#core-subsystems--hardware-drivers)
- [Interactive Desktop Applications](#interactive-desktop-applications)
- [CLI Shell Built-in Commands](#cli-shell-built-in-commands)
- [Directory Structure](#directory-structure)
- [Getting Started & Build Instructions](#getting-started--build-instructions)
- [Automated Testing](#automated-testing)
- [Author & License](#author--license)

---

## Architecture Overview

```
                      +-------------------------------------------------------------+
                      |                 RatanaOS 64-bit (x86_64)                    |
                      |            macOS Sequoia GUI  /  RatanaSH CLI               |
                      +------------------------------+------------------------------+
                                                     |
+----------------------------------------------------v----------------------------------------------------+
|                                         64-bit Kernel Core                                              |
|   +-----------------------+  +------------------------+  +------------------------+  +--------------+   |
|   | 64-bit GDT & TSS      |  | 64-bit 16-byte IDT     |  | 256MB PMM Allocator    |  | 16MB Heap    |   |
|   | (Null, KCode64, KData)|  | (ISRs 0-31, IRQs 0-15) |  | (4KB Page Frames)      |  | (kmalloc)    |   |
|   +-----------------------+  +------------------------+  +------------------------+  +--------------+   |
+----------------------------------------------------+----------------------------------------------------+
                                                     |
+----------------------------------------------------v----------------------------------------------------+
|                                    x86_64 Long Mode Transition                                          |
|   1. Multiboot 1 Protected Mode Entry (32-bit ELF)                                                      |
|   2. Build 4-Level Paging Hierarchy: PML4 -> PDPT -> PD (Identity-map first 1GB with 2MB Huge Pages)    |
|   3. Enable PAE (CR4 bit 5), OSFXSR (CR4 bit 9), OSXMMEXCPT (CR4 bit 10)                                |
|   4. Set Long Mode Enable (LME) in IA32_EFER MSR (0xC0000080 bit 8)                                     |
|   5. Enable Paging & PE in CR0 (bit 31 & bit 0)                                                         |
|   6. Far Jump `jmp 0x08:long_mode_entry` -> Enter Native 64-bit Long Mode with SysV x86_64 ABI          |
+---------------------------------------------------------------------------------------------------------+
```

---

## macOS Sequoia Desktop Environment

```
+-----------------------------------------------------------------------------------------+
| []  Finder  File  Edit  View  Go  Window  Help             [WiFi] [RAM: 24KB] [10:35 PM] | <- Top Menu Bar (24px)
+-----------------------------------------------------------------------------------------+
|                                                                                         |
|  [📁 RatanaOS HD]                                                                       |
|                                                                                         |
|      +-------------------------------------------------------------------+              |
|      | (•)(•)(•)                   About This Mac                        |              |
|      | +---------------------------------------------------------------+ |              |
|      | |       RatanaOS Sequoia (2026 64-bit Edition)                 | |              |
|      | |        Version 15.4 (Build 2026.09)                           | |              |
|      | |        Model:   MacBook Pro (x86_64 Long Mode)                | |              |
|      | |        Chip:    x86_64 Long Mode Processor                    | |              |
|      | |        Memory:  256 MB RAM (16 MB Dynamic Heap)               | |              |
|      | |        Graphics: VBE 32-bit Linear Framebuffer (1024x768)     | |              |
|      | +---------------------------------------------------------------+ |              |
|      +-------------------------------------------------------------------+              |
|                                                                                         |
|                                                                                         |
|             +-------------------------------------------------------------+             |
|             |  [Finder] [Terminal] [SysMon] [Calc] [Paint] [About] [Trash]| <- 48x48 Dock
|             +-------------------------------------------------------------+             |
+-----------------------------------------------------------------------------------------+
```

### Key UI Features:
1. **Top Menu Bar (24px)**:
   - ** Apple Logo**: Dropdown menu with *About This Mac*, *System Settings...*, *Force Quit...*, *Restart...*, and *Exit to CLI*.
   - **Active App Title**: Bold title reflecting the currently focused window (`Finder`, `Terminal`, `Activity Monitor`, `Calculator`, `Paint Studio`, `About This Mac`).
   - **Standard Menus**: `File`, `Edit`, `View`, `Window`, `Help`.
   - **Status Extras (Right Side)**: 📶 Wi-Fi status, 🔋 Battery indicator, 🧠 Dynamic RAM counter (`RAM: XXK`), 🕒 CMOS Real-Time Clock (`MM/DD HH:MM`), 🔍 Spotlight Search, and Control Center icon.
2. **Floating Bottom Dock (Translucent Glass)**:
   - Centered translucent glass container (`alpha=210/255`) with 16px rounded corners.
   - **48x48 Rich Icons**: Finder, Terminal, Activity Monitor, Calculator, Paint Studio, About This Mac, and Trash.
   - **Active Indicators**: Glowing accent dots below open applications.
3. **Window Chrome & Traffic Lights**:
   - 🔴 **Red Button** (`#FF5F56`): Close window.
   - 🟡 **Yellow Button** (`#FFBD2E`): Minimize window to Dock.
   - 🟢 **Green Button** (`#27C93F`): Zoom / Expand window.
   - Centered window titles and alpha-blended drop shadows.
4. **About This Mac System Dialog (480x320)**:
   - 64x64  Apple Emblem.
   - Live hardware specs: CPUID chip model, 256MB RAM, 16MB heap, VBE linear framebuffer resolution, and serial number.

---

## Core Subsystems & Hardware Drivers

| Subsystem | Source File(s) | Description |
| :--- | :--- | :--- |
| **Long Mode Bootloader** | [`src/boot/boot.asm`](file:///home/reny/Documents/OS/src/boot/boot.asm) | Sets up 4-level paging (PML4), PAE, EFER LME, and executes far jump into 64-bit Long Mode |
| **64-bit GDT & IDT** | [`src/kernel/gdt.c`](file:///home/reny/Documents/OS/src/kernel/gdt.c), [`src/kernel/idt.c`](file:///home/reny/Documents/OS/src/kernel/idt.c) | 64-bit code/data descriptors (L=1), 16-byte IDT gates, and assembly interrupt stubs with `iretq` |
| **Memory Management** | [`src/kernel/pmm.c`](file:///home/reny/Documents/OS/src/kernel/pmm.c), [`src/kernel/heap.c`](file:///home/reny/Documents/OS/src/kernel/heap.c) | 256MB Physical Memory Bitmap Allocator (4KB frames) + 16MB 64-bit Dynamic Heap (`kmalloc`, `kfree`) |
| **Linear Framebuffer & Compositor** | [`src/drivers/gfx.c`](file:///home/reny/Documents/OS/src/drivers/gfx.c) | High-res 1024x768x32 linear framebuffer, BGA auto-detection, alpha blending, double buffering (60 FPS) |
| **Icon Engine** | [`src/drivers/icons.c`](file:///home/reny/Documents/OS/src/drivers/icons.c) | 48x48 Dock icons, 64x64 Apple emblem, 24x24 menu bar extras, and vector shapes |
| **PS/2 Mouse Driver** | [`src/drivers/mouse.c`](file:///home/reny/Documents/OS/src/drivers/mouse.c) | Hardware IRQ12 interrupt driver, 3-byte packet decoder, screen clamping, and animated cursor |
| **PS/2 Keyboard Driver** | [`src/drivers/keyboard.c`](file:///home/reny/Documents/OS/src/drivers/keyboard.c) | IRQ1 US-QWERTY keymap with Shift, Caps Lock, and extended scancode `0xE0` arrow keys |
| **CMOS Real-Time Clock** | [`src/drivers/rtc.c`](file:///home/reny/Documents/OS/src/drivers/rtc.c) | Reads hardware RTC registers via ports `0x70/0x71` with dual-read consistency loop |
| **PCI Bus Scanner** | [`src/drivers/pci.c`](file:///home/reny/Documents/OS/src/drivers/pci.c) | Scans all 256 buses, 32 devices, 8 functions via ports `0xCF8/0xCFC` |
| **CPUID Feature Detector** | [`src/kernel/cpuid.c`](file:///home/reny/Documents/OS/src/kernel/cpuid.c) | Queries vendor string, processor brand name, family, model, stepping, and feature flags |
| **PC Speaker Driver** | [`src/drivers/speaker.c`](file:///home/reny/Documents/OS/src/drivers/speaker.c) | Synthesizes square waves via PIT channel 2 and port `0x61` (`beep`) |
| **Serial COM1 Logger** | [`src/drivers/serial.c`](file:///home/reny/Documents/OS/src/drivers/serial.c) | 115200 baud UART logging via port `0x3F8` |

---

## Interactive Desktop Applications

1. **Finder**: Split-view file browser with favorites sidebar (`Applications`, `Desktop`, `Documents`, `Downloads`) and grid items (`RatanaOS HD`, `System`, `Applications`, `Library`).
2. **Terminal.app**: Dark console window running `RatanaSH` with zsh-like prompt and neofetch ASCII info.
3. **Activity Monitor.app**: CPU performance monitor, memory usage visual bars, uptime, and process list table.
4. **Calculator.app**: macOS-style calculator with clickable button matrix (`0-9`, `+`, `-`, `*`, `/`, `=`, `C`).
5. **Paint Studio.app**: Interactive creative drawing canvas with 8-color palette selector and freehand brush drawing.
6. **About This Mac**: Authentic macOS system specification dialog.
7. **Trash**: Wastebasket icon in Dock (clears drawing canvas when clicked).

---

## CLI Shell Built-in Commands

When operating in text mode or inside `Terminal.app`:

```
ratana@os-x86_64 > help

RatanaOS 64-bit (x86_64) Built-in Commands:
  gui               - Launch Full-Feature 64-bit macOS GUI Desktop
  fetch             - System overview & 64-bit ASCII architecture
  date / time       - Query hardware CMOS Real-Time Clock (2026)
  mem / free        - 64-bit Physical Memory & Dynamic Heap stats
  pci               - Scan and enumerate all PCI bus devices
  cpuid             - Query 64-bit CPU architecture and feature flags
  calc <a> <op> <b> - Arithmetic calculator (+, -, *, /, %)
  beep [freq] [ms]  - Play tone through PC speaker (default 440Hz 200ms)
  theme <name>      - Apply theme (arch, cyber2026, matrix, ocean, amber)
  matrix            - Animated digital rain screensaver
  snake             - Play interactive Snake arcade game
  color <fg> [bg]   - Set terminal colors (0-15)
  clear             - Clear terminal screen
  echo <text>       - Print text to console
  uptime            - Display system uptime and PIT timer ticks
  about             - System and developer details
  reboot            - Pulse CPU reset line
  halt              - Halt processor execution
```

---

## Directory Structure

```
RatanaOS/
├── Makefile                     # Build system (64-bit GCC, NASM, LD, QEMU runner, test suite)
├── README.md                    # Main project documentation
├── docs/
│   └── ARCHITECTURE.md          # Detailed 64-bit kernel & GUI technical specification
└── src/
    ├── boot/
    │   ├── boot.asm             # 4-Level Paging, Long Mode entry, and 64 KiB stack
    │   └── linker.ld            # 64-bit ELF linker script aligning at 1MB
    ├── drivers/
    │   ├── gfx.c                # Linear framebuffer engine with alpha blending & wallpaper buffer
    │   ├── icons.c              # 48x48 Dock icons, 64x64 Apple emblem, 24x24 status extras
    │   ├── keyboard.c           # PS/2 Keyboard IRQ1 driver with extended scancode 0xE0
    │   ├── mouse.c              # PS/2 Mouse IRQ12 driver with 3-byte packet decoder & cursor
    │   ├── pci.c                # PCI configuration space scanner
    │   ├── rtc.c                # CMOS Real-Time Clock driver
    │   ├── serial.c             # Serial COM1 (0x3F8) debug logger
    │   ├── speaker.c            # PC Speaker audio synthesizer
    │   └── vga.c                # VGA 80x25 text mode driver
    ├── include/
    │   ├── cpuid.h              # CPUID prototypes and feature structures
    │   ├── dock.h               # Floating bottom dock geometry and hit testing
    │   ├── font8x16.h           # 128-character 8x16 VGA ASCII bitmap font table
    │   ├── gdt.h                # 64-bit GDT structures
    │   ├── gfx.h                # 32-bit ARGB colors, alpha blending, and drawing primitives
    │   ├── gui.h                # Window manager, traffic light buttons, and desktop apps
    │   ├── heap.h               # 64-bit Dynamic Kernel Heap allocator
    │   ├── icons.h              # Vector and procedural icon renderers
    │   ├── idt.h                # 16-byte 64-bit IDT gate structures
    │   ├── io.h                 # Port I/O inline assembly (inb, outb, inw, outw)
    │   ├── isr.h                # 64-bit CPU register context and interrupt dispatcher
    │   ├── keyboard.h           # Keyboard state and scancode definitions
    │   ├── menubar.h            # 24px top menu bar and Apple menu dropdown
    │   ├── mouse.h              # Mouse coordinate tracking and button state
    │   ├── multiboot.h          # Multiboot 1 specification structures
    │   ├── pci.h                # PCI bus reading and class definitions
    │   ├── pic.h                # Dual 8259 PIC initialization and EOI
    │   ├── pmm.h                # 256MB Physical Memory Bitmap Allocator
    │   ├── rtc.h                # Real-Time Clock time structure
    │   ├── serial.h             # COM1 serial driver
    │   ├── shell.h              # CLI interactive shell interface
    │   ├── snake.h              # Snake arcade game
    │   ├── speaker.h            # PC Speaker tone API
    │   ├── stdio.h              # Standard I/O (kprintf with %s, %d, %u, %x, %p)
    │   ├── string.h             # Standard string functions (strlen, strcmp, itoa, utoa64)
    │   ├── timer.h              # 8254 PIT timer driver
    │   ├── types.h              # Standard 64-bit types (uint64_t, uintptr_t, size_t)
    │   └── vga.h                # VGA text mode definitions
    ├── kernel/
    │   ├── cpuid.c              # CPUID processor brand & feature detection
    │   ├── dock.c               # macOS Floating Dock implementation
    │   ├── gdt.c                # 64-bit GDT loader
    │   ├── gdt_flush.asm        # 64-bit GDT reload with retfq
    │   ├── gui.c                # Window Manager, traffic light controls, and 6 desktop apps
    │   ├── heap.c               # 64-bit heap manager (kmalloc, kfree, coalescing)
    │   ├── idt.c                # 256-entry IDT initializer
    │   ├── interrupts.asm       # 64-bit ISR & IRQ assembly stubs with iretq
    │   ├── isr.c                # Exception and interrupt dispatcher
    │   ├── kernel.c             # 64-bit Kernel Main entry point
    │   ├── matrix.c             # Matrix digital rain screensaver
    │   ├── menubar.c            # 24px macOS Top Menu Bar implementation
    │   ├── pic.c                # 8259 PIC remapping
    │   ├── pmm.c                # 256MB Physical Memory Page Allocator
    │   ├── shell.c              # Interactive CLI command executor
    │   ├── snake.c              # Interactive Snake arcade game
    │   └── timer.c              # 8254 PIT calibrated at 100 Hz
    └── lib/
        ├── stdio.c              # Formatted kernel printing
        └── string.c             # String and memory manipulation library
```

---

## Getting Started & Build Instructions

### Prerequisites (Arch Linux / Ubuntu / Debian / macOS)
Install the required build tools:
```bash
# Arch Linux / Garuda / Manjaro:
sudo pacman -S base-devel nasm qemu-system-x86_64 xorriso grub

# Ubuntu / Debian:
sudo apt update && sudo apt install build-essential nasm qemu-system-x86 xorriso grub-pc-bin grub-common
```

### 1. Build the 64-bit Kernel
```bash
make clean && make
```
*Output: `build/ratanaos.bin` (native 64-bit ELF) and `build/ratanaos32.bin` (Multiboot direct loader).*

### 2. Run in QEMU
```bash
make run
```
*This starts QEMU in high-resolution graphics mode with serial COM1 redirected to stdio.*

### 3. Build & Run Bootable ISO
```bash
make run-iso
```

### 4. Run in Terminal (Curses Mode)
```bash
make run-curses
```

---

## Automated Testing

RatanaOS includes an automated test suite verifying Multiboot compliance, 64-bit ELF header alignment, and symbol table integrity:

```bash
make test
```

Test Results:
```
==============================================
     RATANAOS 64-BIT MACOS TEST SUITE         
==============================================

[TEST 1] Multiboot Header Verification...
  [PASS] Multiboot header is valid and compliant.

[TEST 2] 64-bit ELF Layout & Section Alignment...
  Class:                             ELF64
  Machine:                           Advanced Micro Devices X86-64
  Entry point address:               0x100010
  [PASS] Native ELF 64-bit x86-64 executable layout validated.

[TEST 3] 64-bit Kernel & macOS GUI Symbols...
  [PASS] 64-bit macOS GUI & icon symbols verified.

==============================================
   ALL 64-BIT MACOS TESTS PASSED!             
==============================================
```

---

## Author & License

- **Author**: [Ratanazen](https://github.com/Ratanazen)
- **Repository**: [https://github.com/Ratanazen/RatanaOS.git](https://github.com/Ratanazen/RatanaOS.git)
- **License**: MIT License - open-source for educational and hobby OS development.
