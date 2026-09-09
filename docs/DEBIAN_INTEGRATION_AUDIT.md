# RatanaOS Debian Integration Audit

## 1. Overview
This document serves as the architecture gap report for transforming RatanaOS into a Debian-compatible ecosystem while retaining its native x86_64 kernel.

## 2. Component Audit

| Component | Current State | Classification | Next Steps |
|-----------|---------------|----------------|------------|
| **Boot Process** | Multiboot 1/2 compatible (`boot.asm`, `linker.ld`) | **KEEP** | Extend to pass memory map to PMM/VMM. |
| **Kernel Entry** | `kernel.c` initializes basic drivers and launches GUI | **EXTEND** | Refactor to initialize VMM, VFS, Scheduler, then load `init` from initramfs. |
| **Memory Management** | `pmm.c` (Physical), `heap.c` (Kernel Heap) | **EXTEND** | Implement Virtual Memory Manager (`vmm.c`), Paging, and Userspace mappings. |
| **GDT / IDT** | `gdt.c`, `idt.c`, `isr.c` (Basic ring 0 setup) | **EXTEND** | Add TSS (Task State Segment) for Ring 3 userspace switches. |
| **Interrupts/Timer** | `pic.c`, `timer.c` (PIT/APIC) | **EXTEND** | Hook timer to the process scheduler for preemption. |
| **Input Drivers** | `keyboard.c`, `mouse.c` | **KEEP** | Expose via `/dev/input/` once VFS is implemented. |
| **Graphics** | `gfx.c`, `vga.c` (Linear framebuffer) | **EXTEND** | Expose via `/dev/fb0`. Prevent direct kernel access from userspace. |
| **Hardware** | `pci.c`, `rtc.c`, `serial.c`, `speaker.c` | **KEEP** | Modularize into a proper driver architecture. |
| **GUI & Shell** | `gui.c`, `shell.c`, `dock.c`, `menubar.c` | **REWRITE** | Currently compiled into the kernel. Must be ported to userspace over IPC. |
| **File System** | `packages.c` (In-memory mock FS) | **REPLACE** | Implement full VFS (`vfs.c`), initramfs, and FAT32/EXT2 drivers. |
| **Process Model** | Monolithic kernel thread | **NOT_IMPLEMENTED** | Create `process.c`, PID management, Context Switching, and Scheduler. |
| **Syscalls** | None | **NOT_IMPLEMENTED** | Implement `syscall.c` (Ring 3 to Ring 0 ABI) starting with `fork`, `exec`, `write`. |
| **Userspace** | None | **NOT_IMPLEMENTED** | Create `libc`, ELF Loader, and compile applications as standalone binaries. |
| **Build System** | `Makefile` (Single binary `ratanaos.bin`) | **EXTEND** | Split into `make kernel`, `make libc`, `make userspace`, `make initramfs`. |

## 3. Architecture Gap Summary
Currently, RatanaOS is a "monolithic unikernel" where the GUI, Shell, and Applications run in Ring 0 along with the hardware drivers. To achieve Debian ecosystem compatibility, we must introduce a strict User/Kernel boundary (Ring 3 vs Ring 0).

**The immediate missing foundations are:**
1. **Virtual Memory (Paging)**: To isolate processes.
2. **Task State Segment (TSS)**: To safely transition between User Mode and Kernel Mode.
3. **Syscall Interface**: To allow userspace to request kernel services (e.g., `write`, `exit`).
4. **Process Scheduler**: To multitask userspace programs.
5. **VFS & Initramfs**: To load the first ELF executable (`/sbin/init`) from a filesystem.
6. **ELF Loader**: To parse and execute standard `.o`/`.bin` files.

## 4. Phase 1 Migration Plan (First Working Milestone)
Before porting the GUI or networking, we will execute **PHASE 36 (FIRST WORKING MILESTONE)**:
1. Scaffold the new directory structure (`src/mm`, `src/process`, `src/syscall`, `src/fs`, `src/libc`).
2. Implement Virtual Memory (`vmm.c`).
3. Implement VFS and a simple Initramfs.
4. Implement Process creation and Context Switching.
5. Implement Syscall entry point (`syscall_entry.asm`) and basic syscalls (`write`, `exit`).
6. Build a minimal `libc` and load a userspace `hello world` ELF binary.
