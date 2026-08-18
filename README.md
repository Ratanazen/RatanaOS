# RatanaOS (32-bit x86 Operating System)

![Architecture](https://img.shields.io/badge/Architecture-x86_%7C_i686-blue)
![Language](https://img.shields.io/badge/Language-C23_%2F_NASM-orange)
![License](https://img.shields.io/badge/License-MIT-green)

**RatanaOS** is a modular 32-bit x86 operating system kernel developed from scratch with low-level hardware drivers, interrupt management, and an interactive shell.

---

## Features & Subsystems

- **Bootloader**: Multiboot-compliant assembly bootloader supporting GRUB and direct QEMU execution.
- **Memory Management**: 5-segment Global Descriptor Table (GDT) for Kernel/User Code & Data segmentation.
- **Interrupts & Exceptions**:
  - 256-entry Interrupt Descriptor Table (IDT).
  - CPU Exception handlers (Divide by zero, GPF, Page Fault, etc.) with detailed panic registers display.
  - Dual 8259 PIC remapping (IRQs 0–15).
- **Timers**: 8254 PIT (Programmable Interval Timer) calibrated at 100 Hz for system ticks and `timer_sleep_ms`.
- **Drivers**:
  - **VGA Text Driver**: 80x25 screen with full 16-color palette, cursor control, backspace handling, and smooth hardware scrolling.
  - **PS/2 Keyboard Driver**: Interrupt-driven (IRQ1) with US-QWERTY key mapping, Shift, and Caps Lock support.
- **Kernel Standard Library (`libk`)**:
  - `string.h`: `strlen`, `strcmp`, `strncmp`, `strcpy`, `strcat`, `memset`, `memcpy`, `itoa`, `atoi`.
  - `stdio.h`: Formatted `kprintf` supporting `%s`, `%d`, `%u`, `%x`, `%X`, `%c`, `%p`.
- **Interactive Shell (`RatanaSH`)**:
  - `fetch`: Custom Neofetch-style system info banner with RatanaOS ASCII logo.
  - `help`: Built-in command manual.
  - `calc <num1> <op> <num2>`: Arithmetic calculator (`+`, `-`, `*`, `/`, `%`).
  - `color <fg> [bg]`: Change VGA terminal color scheme in real time.
  - `echo <text>`: Print text.
  - `uptime`: Show uptime in seconds and timer ticks.
  - `about`: Developer info and kernel specifications.
  - `reboot`: ACPI / 8042 keyboard controller pulse reboot.
  - `halt`: Halt CPU.

---

## Directory Structure

```text
RatanaOS/
├── Makefile                # Build and execution targets
├── README.md               # Documentation and quickstart
├── iso/                    # GRUB ISO generation directory
│   └── boot/grub/grub.cfg
└── src/
    ├── boot/
    │   ├── boot.asm        # Multiboot header & entry point
    │   └── linker.ld       # 1MB alignment linker script
    ├── drivers/
    │   ├── keyboard.c      # PS/2 keyboard driver
    │   └── vga.c           # VGA text mode display driver
    ├── include/            # Header definitions
    │   ├── gdt.h
    │   ├── idt.h
    │   ├── io.h
    │   ├── isr.h
    │   ├── keyboard.h
    │   ├── pic.h
    │   ├── shell.h
    │   ├── stdio.h
    │   ├── string.h
    │   ├── timer.h
    │   ├── types.h
    │   └── vga.h
    ├── kernel/
    │   ├── gdt.c           # GDT setup
    │   ├── gdt_flush.asm   # GDT segment register flush
    │   ├── idt.c           # IDT initialization
    │   ├── interrupts.asm  # ISR & IRQ low-level assembly stubs
    │   ├── isr.c           # Exception & IRQ dispatcher
    │   ├── kernel.c        # Main kernel initialization sequence
    │   ├── pic.c           # 8259 PIC remapping & EOI
    │   ├── shell.c         # Interactive CLI & commands
    │   └── timer.c         # PIT timer (100 Hz)
    └── lib/
        ├── stdio.c         # Kernel formatted printing (kprintf)
        └── string.c        # String & memory operations
```

---

## Building & Running (Arch / Garuda Linux)

### 1. Requirements
Ensure build tools are installed:
```bash
sudo pacman -S gcc nasm qemu-system-x86 grub
```

### 2. Build Kernel
```bash
make build
```

### 3. Run in QEMU
```bash
make run
```

### 4. Build Bootable ISO
```bash
make iso
make run-iso
```
