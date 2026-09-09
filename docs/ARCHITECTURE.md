# 🏛️ RatanaOS System Architecture

**RatanaOS** is a native, freestanding 64-bit (x86_64) operating system featuring a high-performance modular kernel, polymorphic Virtual File System (VFS), POSIX userspace runtime (`libc`), interactive shell and coreutils, Intel e1000 networking stack, and macOS Sequoia-inspired GUI.

---

## High-Level Architecture Diagram

```text
                                 RATANAOS
                                    │
               ┌────────────────────┴────────────────────┐
               │                                         │
        KERNEL SUBSYSTEMS                       USERSPACE SUBSYSTEMS
               │                                         │
    ┌──────────┼──────────┐                   ┌──────────┼──────────┐
    │          │          │                   │          │          │
   CPU         MM         FS                 libc      shell       GUI
    │          │          │                   │          │          │
 GDT/IDT     Paging      VFS                POSIX ABI  coreutils  Compositor
 APIC/PIC    Heap/Slab   DevFS / ProcFS     crt0.o     init/sh    Dock/Menubar
 ACPI/SMP    VMM User    Initramfs / Ext2   sys_brk    ratapkg    Settings.app
    │
 DRIVERS & NETWORKING
    │
 PCI / ATA / RTC / Serial / e1000 NIC / IPv4 / ARP / ICMP
```

---

## Subsystem Breakdown

### 1. Boot & CPU Initialization
- **Entry**: 32-bit Multiboot entry (`src/boot/boot.asm`) transitioning CPU to 64-bit Long Mode with PAE and identity paging.
- **Descriptors**: Global Descriptor Table (`src/kernel/gdt.c`, `src/kernel/gdt_flush.asm`), Task State Segment (TSS), Interrupt Descriptor Table (`src/kernel/idt.c`), Interrupt Service Routines (`src/kernel/isr.c`, `src/kernel/interrupts.asm`).
- **Interrupt Routing**: Dual 8259 PIC remapped to vectors 0x20–0x2F; ACPI MADT / Local APIC detection.
- **ACPI Subsystem**: `src/kernel/acpi.c` detects RSDP in EBDA / BIOS ROM, parses RSDT/XSDT, MADT (CPU core enumeration and I/O APIC), and FADT for poweroff and reboot.

### 2. Memory Management (MM)
- **Physical Memory**: Bitmap page frame allocator (`src/kernel/mm/physical.c`) managing 4 KiB physical frames.
- **Virtual Memory**: 4-level PML4 paging (`src/kernel/mm/virtual.c`) with 4GB identity mapped kernel space and user/supervisor protection bits.
- **Kernel Heap**: Dynamic kernel memory allocator (`src/kernel/mm/heap.c`) providing `kmalloc()` and `kfree()`.

### 3. Process Management & Syscalls
- **Process Control Block (PCB)**: `process_t` tracking PID, PPID, CR3 address space, process state (`RUNNING`, `READY`, `SLEEPING`, `ZOMBIE`), and per-process File Descriptor table (`fds[MAX_FDS]`).
- **Context Switching**: Cooperative and preemptive timer-driven round-robin scheduler (`src/kernel/process/process.c`).
- **Syscall ABI**: `int $0x80` software interrupt interface supporting `read`, `write`, `open`, `close`, `stat`, `fstat`, `lseek`, `mmap`, `brk`, `pipe`, `yield`, `dup`, `dup2`, `getpid`, `fork`, `execve`, `exit`, `wait4`, `uname`, `getcwd`, `chdir`.

### 4. Virtual File System (VFS) & Storage
- **Polymorphic Device Nodes**: Node operations (`read`, `write`, `open`, `close`) abstracted per filesystem type.
- **DevFS**: Pseudo-devices `/dev/null`, `/dev/zero`, `/dev/random`, `/dev/urandom`, `/dev/console`, `/dev/tty`.
- **ProcFS**: Kernel information filesystem exposing `/proc/cpuinfo`, `/proc/meminfo`, `/proc/uptime`, `/proc/version`.
- **IPC Pipes**: In-kernel FIFO ring buffer pipes created via `pipe()`.
- **Initramfs**: Embedded ELF64 binary image mounted at root (`/sbin/init`, `/bin/*`).
- **Ext2 & ATA**: ATA PIO block storage driver with Ext2 filesystem parser (`src/kernel/fs/ext2.c`, `src/drivers/ata.c`).

### 5. Network Stack & Drivers
- **Hardware**: Intel 82540EM / 82545EM / 82543GC PCI Ethernet controller driver (`src/drivers/e1000.c`).
- **Ethernet Framing**: Standard 14-byte 802.3 Ethernet frame construction and demultiplexing (`src/kernel/net/net.c`).
- **ARP Subsystem**: Dynamic ARP request / reply handling and 32-entry ARP cache table.
- **IPv4 & ICMP**: IPv4 packet routing and ICMP Echo Request / Echo Reply (ping) engine.

### 6. Userspace Runtime (`libc`) & Core Utilities
- **C Runtime**: Standalone `crt0.o` entry point calling `main(argc, argv)` and terminating with `exit()`.
- **Userspace Libc**: Standard `malloc`/`free` backed by `sys_brk`, formatted I/O (`printf`, `sprintf`, `snprintf`), string manipulation (`strlen`, `strcmp`, `strcpy`, `memcpy`, `memset`), and POSIX syscall wrappers (`unistd.h`).
- **Native Core Utilities**:
  - `/sbin/init`: Process #1 orchestrator
  - `/bin/sh`: Interactive shell with pipelines, redirection, environment, builtins (`cd`, `pwd`, `echo`, `uname`, `help`, `exit`), and external program execution
  - `/bin/ls`, `/bin/cat`, `/bin/echo`, `/bin/pwd`, `/bin/uname`, `/bin/touch`, `/bin/mkdir`, `/bin/rm`, `/bin/free`, `/bin/ps`, `/bin/date`, `/bin/clear`
  - `/bin/ratapkg`: Native package management tool (`.rpk`)
  - `/bin/debimport`: Debian `.deb` package importer and compatibility analyzer

### 7. macOS Sequoia GUI & Compositor
- **Graphics Engine**: 32-bit ARGB software renderer (`src/drivers/gfx.c`) with double buffering, alpha blending, and geometry primitives.
- **Icon Rendering Pipeline**: Pre-rendered asset tables and dynamic bilinear scaling (`src/drivers/icons.c`, `src/drivers/icons_assets.c`).
- **Desktop Environment**: Translucent menu bar with Apple menu, dynamic dock with active indicators and bounce animations, multi-window manager, and native tabbed `System Settings.app`.
