# RatanaOS (64-bit x86_64 Long Mode Edition)

![Year](https://img.shields.io/badge/Edition-64--bit_Long_Mode-blueviolet)
![Architecture](https://img.shields.io/badge/Architecture-x86__64_%7C_AMD64-blue)
![Graphics](https://img.shields.io/badge/GUI-VBE_32--bit_Framebuffer-brightgreen)
![Language](https://img.shields.io/badge/Language-C23_%2F_NASM_64-orange)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen)
![License](https://img.shields.io/badge/License-MIT-green)

**RatanaOS 64-bit** is a high-performance operating system kernel operating natively in **64-bit x86_64 Long Mode**. It features a 4-level paging memory manager (PML4), 64-bit GDT/IDT, 64-bit dynamic heap allocator, hardware PS/2 mouse & keyboard drivers, and a full **Linux-like Graphical User Interface (GUI) Desktop Environment**.

---

## 64-bit Kernel Architecture

```
                      +------------------------------------------+
                      |        RatanaOS 64-bit (x86_64)          |
                      |       (CLI Shell & RatanaWM GUI)         |
                      +--------------------+---------------------+
                                           |
+------------------------------------------v------------------------------------------+
|                                64-bit Kernel Core                                   |
|   +---------------------+  +----------------------+  +--------------------------+   |
|   | 64-bit GDT & TSS    |  | 64-bit 16-byte IDT   |  | 64-bit PMM & 16MB Heap   |   |
|   | (Null, KCode, KData)|  | (ISRs & IRQs, iretq) |  | (64-bit Virtual Space)   |   |
|   +---------------------+  +----------------------+  +--------------------------+   |
+------------------------------------------+------------------------------------------+
                                           |
+------------------------------------------v------------------------------------------+
|                           x86_64 Long Mode Transition                               |
|   1. Verify CPUID Long Mode Support (EFER / MSR 0xC0000080)                         |
|   2. Build 4-Level Paging Hierarchy: PML4 -> PDPT -> PD (2MB Large Pages)           |
|   3. Enable PAE (CR4 bit 5) -> Enable LME in EFER MSR -> Enable Paging (CR0 bit 31)  |
|   4. 64-bit Far Jump to Code Segment 0x08 -> Entry into Native 64-bit Long Mode     |
+-------------------------------------------------------------------------------------+
```

---

## Subsystems & Features

### 1. 64-bit Memory & CPU Management
- **4-Level Paging**: PML4, PDPT, and Page Directory identity-mapping the first 1GB with 2MB huge pages.
- **Physical Memory Manager (PMM)**: 256MB Physical Memory Bitmap frame allocator.
- **Dynamic Kernel Heap**: 16MB 64-bit heap (`kmalloc`, `kcalloc`, `krealloc`, `kfree`) with 16-byte alignment.
- **64-bit GDT & IDT**: 16-byte IDT gate descriptors, 64-bit ISR/IRQ stubs with register preservation and `iretq`.

### 2. Full 64-bit GUI Desktop Environment (`RatanaWM`)
- High-resolution `1024x768x32` linear framebuffer graphics engine with double buffering (`60 FPS`).
- Hardware PS/2 mouse driver (IRQ12) with screen boundary clipping and cursor rendering.
- Movable windows with active focus management, titlebars, and **Close [X]** / **Minimize [_]** controls.
- Bottom Taskbar with **Start Menu**, active window tabs, **RTC 2026 Clock**, and **RAM Usage Widget**.
- Built-in Applications: **Terminal Console**, **System Monitor**, **GUI Calculator**, **Paint Canvas**, and **About RatanaOS**.

### 3. Hardware Drivers & Shell Utilities
- **CMOS RTC**: Accurate hardware clock reporting Year 2026 timestamps (`date` / `time`).
- **Drivers**: Serial COM1 (`0x3F8`), PC Speaker synthesizer (`beep`), PCI bus enumerator (`pci`), and CPUID inspector (`cpuid`).
- **Arcade & Visuals**: Matrix digital rain animation (`matrix`) and playable Snake game (`snake`).

---

## Quickstart & Testing

### 1. Build 64-bit Kernel
```bash
make clean && make
```

### 2. Run Automated 64-bit Test Suite
```bash
make test
```

### 3. Run in QEMU
```bash
make run
```
*Tip: Once the system boots, type `gui` in the shell to launch the desktop environment, or press `ESC` inside the GUI to return to CLI.*

### 4. Run in Terminal (Curses Mode)
```bash
make run-curses
```
