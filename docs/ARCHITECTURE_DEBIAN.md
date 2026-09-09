# 🏛️ RatanaOS Debian Architecture & Integration Report (v3.0)

This document serves as the architectural reference and audit report for integrating **Debian GNU/Linux 13 "Trixie" (x86_64 / amd64)** into **RatanaOS**.

---

## 1. Current RatanaOS Subsystem Architecture

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

- **Bootloader**: Multiboot 1 standard interface (i386-pc BIOS), GRUB 2 bootloader. Long mode CPU initialization in `src/boot/boot.asm` via PML4 identity paging (0 to 4GB).
- **Kernel Core**: Freestanding 64-bit x86-64 C kernel (`src/kernel/kernel.c`). GDT (`gdt.c`), IDT (`idt.c`), 8259 PIC (`pic.c`), APIC/ACPI table parser (`acpi.c`), Serial COM1 logger.
- **Memory Management**: Bitmap Physical Frame Allocator (`physical.c`, 4KB frames), 4-Level PML4 Paging (`virtual.c`, 0-4GB identity mapping, page tables), Kernel Heap & Slab Allocator (`heap.c`, `kmalloc`/`kfree`).
- **Virtual File System (VFS)**: Polymorphic VFS (`vfs.c`), DevFS (`/dev/null`, `/dev/zero`, `/dev/console`, `/dev/tty`), ProcFS (`/proc/cpuinfo`, `/proc/meminfo`, `/proc/uptime`, `/proc/version`), Initramfs embedded RAM disk (`/sbin/init`, coreutils).
- **Filesystems**: Ext2 read-only file/directory parser (`ext2.c`) with ATA PIO disk driver (`ata.c`); DebianFS container reader (`debianfs.c`) mounting `DEBF` container image at `/system/debian` and `/mnt/debian`.
- **ELF & ABI**: ELF64 parser and loader (`elf.c`) supporting 64-bit x86-64 executables (ET_EXEC, ET_DYN PT_LOAD, PT_INTERP dynamic loader linking `ld-linux-x86-64.so.2`). Dual Syscall ABI: (1) Native RatanaOS `int $0x80` gate; (2) x86-64 `SYSCALL`/`SYSRET` MSR gate (`LSTAR` MSR `0xC0000102`, `STAR`, `FMASK`, `KernelGSBase`).
- **Process Management**: `process_t` PCB tracking PID, PPID, CR3 PML4 page table root, process status, context switch registers, per-process FDs (`fds[MAX_FDS]`). Preemptive & cooperative round-robin scheduler (`process.c`).
- **macOS Sequoia GUI**: macOS Sequoia-inspired 32-bit ARGB software rendering engine (`gfx.c`), double-buffered compositor, translucent menu bar (`menubar.c`), animatable dock (`dock.c`), windowing system (`gui.c`), System Settings app, WhiteSur / MacTahoe / Vector icon system (`icons.c`).

---

## 2. Debian Integration Points

1. **Track A — Dual Boot**: GRUB 2 configuration (`iso/boot/grub/grub.cfg`) featuring entries for RatanaOS 64-bit, Safe Text Mode, host Debian partition chainloading (`DEBIAN_ROOT` / `DEBIAN_UUID`), and `make iso-with-debian DEBIAN_ISO=...` embedded installer ISO support.
2. **Track B — Embedded Debian RootFS**: Python packaging tool (`tools/bake_debian_rootfs.py`) with security path traversal checks (`validate_path`) baking rootfs into `DEBF` container image (`src/kernel/fs/debian_data.c`). Read-only VFS mount at `/system/debian` and `/mnt/debian`.
3. **Track C — Linux Compatibility Layer**: Dedicated Linux system call entry point (`syscall_entry_stub` via MSR `0xC0000082` `LSTAR`) and dispatcher (`syscall_handler_linux`). 26 Linux system calls mapped (`read`, `write`, `open`, `close`, `stat`, `fstat`, `brk`, `mmap`, `arch_prctl`, `exit`, `exit_group`, etc.).

---

## 3. Missing Components & Limitations

- **UEFI Booting**: Native UEFI booting of RatanaOS kernel is not implemented (requires Multiboot 1 / BIOS).
- **Ext4 Kernel Driver**: Ext4 filesystem write support is unavailable in kernel space (RootFS is read-only).
- **Process Namespaces**: Full Linux chroot path isolation requires complete VFS root remapping for `/system/debian` path resolution.
- **Complex Syscalls**: Advanced Linux syscalls (`clone` flags, `epoll`, `socketcall`, signal stack unwinding) are partially implemented.

---

## 4. Implementation Plan & Success Levels

```text
LEVEL 0: RatanaOS only                           [PASSED]
LEVEL 1: Debian Dual Boot                        [PASSED]
LEVEL 2: Debian RootFS readable                 [PASSED]
LEVEL 3: Linux static ELF executable             [PASSED]
LEVEL 4: Static Debian binary executable         [PASSED]
LEVEL 5: Dynamic Linux ELF executable            [PASSED]
LEVEL 6: glibc-compatible Debian application     [IN PROGRESS]
LEVEL 7: Debian shell                            [IN PROGRESS]
LEVEL 8: Meaningful Debian userspace             [TARGET]
LEVEL 9: Advanced Debian application compat      [TARGET]
```
