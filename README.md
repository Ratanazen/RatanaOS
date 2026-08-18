# RatanaOS (2026 Full GUI Desktop Edition)

![Year](https://img.shields.io/badge/Edition-2026_Full_GUI_Desktop-blueviolet)
![Architecture](https://img.shields.io/badge/Architecture-x86_%7C_i686-blue)
![Graphics](https://img.shields.io/badge/GUI-VBE_32--bit_Framebuffer-brightgreen)
![Language](https://img.shields.io/badge/Language-C23_%2F_NASM-orange)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen)
![License](https://img.shields.io/badge/License-MIT-green)

**RatanaOS 2026** is a modular 32-bit x86 operating system kernel featuring a full **Linux-like Graphical User Interface (GUI) Desktop Environment**, hardware PS/2 mouse support (IRQ12), VBE 32-bit linear framebuffer rendering, memory management, and built-in desktop applications.

---

## GUI Desktop Environment (`RatanaWM`)

```
+-----------------------------------------------------------------------------+
|  +--------------------+  +--------------------+  +--------------------+     |
|  | Terminal Console   |  | System Monitor     |  | Calculator / Paint |     |
|  | (ratana@os-2026 >) |  | (CPU, RAM, Uptime) |  | (Clickable Buttons)|     |
|  +--------------------+  +--------------------+  +--------------------+     |
|                                                                             |
|                                                     [RatanaOS 2026]         |
+-----------------------------------------------------------------------------+
| [RatanaOS Start] | [Terminal] [SysMon] [Calc] | [RAM: 24KB] | [2026-08-18]  |
+-----------------------------------------------------------------------------+
```

### Desktop Features & Applications:
1. **Window Manager (`RatanaWM`)**:
   - Draggable windows with active focus management and Z-order layering.
   - Titlebars with **Close [X]** and **Minimize [_]** buttons.
2. **Taskbar & Start Menu**:
   - **Start Menu**: App launcher popup and option to exit back to CLI.
   - **Active Window Tabs**: Taskbar buttons to switch, restore, or minimize windows.
   - **RTC 2026 Clock Widget**: Live real-time clock synchronized with CMOS hardware.
   - **RAM Usage Widget**: Real-time memory allocation indicator.
3. **Built-in Desktop Applications**:
   - **Terminal Console**: Live interactive terminal with commands.
   - **System Monitor**: Visual resource bars for Physical 128MB RAM and 8MB Dynamic Heap.
   - **GUI Calculator**: Clickable button matrix (`0-9`, `+`, `-`, `*`, `/`, `=`, `C`).
   - **Paint Canvas**: Color palette bar and freehand brush drawing with mouse drag.
   - **About RatanaOS**: System information and specifications.

---

## Core Subsystems & Hardware Drivers

- **Graphics Engine (`gfx.h/c`)**:
  - VBE 32-bit linear framebuffer (`1024x768x32 bpp`).
  - Smooth double-buffered backbuffer swap (`60 FPS` tear-free).
  - Primitives: rectangles, outlines, gradients, circles, lines, and embedded 8x16 bitmap font.
- **PS/2 Mouse Driver (`mouse.h/c`)**:
  - Interrupt-driven (IRQ12) with 3-byte packet decoding.
  - Coordinate tracking with screen boundary clamping and hardware cursor rendering.
- **Memory Management**:
  - 128MB Physical Memory Bitmap Allocator (PMM).
  - 8MB Dynamic Kernel Heap (`kmalloc`, `kcalloc`, `krealloc`, `kfree`).
- **Core CPU & Interrupts**:
  - 5-segment GDT, 256-gate IDT, Dual 8259 PIC remapping.
  - 8254 PIT timer (100 Hz), CMOS RTC (2026 timestamps), PC Speaker sound driver.
  - Serial COM1 (`0x3F8`) debug logger, PCI Bus scanner, CPUID inspector.

---

## Quickstart & Testing

### 1. Build Kernel & GUI
```bash
make clean && make
```

### 2. Run Automated Integrity Tests
```bash
make test
```

### 3. Launch in QEMU
```bash
make run
```
*Tip: Once the system boots, type `gui` in the shell to launch the desktop environment, or press `ESC` inside the GUI to return to CLI.*

### 4. Run inside Terminal (Curses Mode)
```bash
make run-curses
```
