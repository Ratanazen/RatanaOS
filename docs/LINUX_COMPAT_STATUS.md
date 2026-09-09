# 🐧 Linux Syscall Compatibility Status (Track C)

This document tracks the implementation status of Linux x86_64 ABI and system call compatibility in the RatanaOS kernel.

---

## 📊 Linux Syscall Status Table

| Linux Syscall Name | Linux Syscall # (x86_64) | Implementation Status | Routing Subsystem / Notes |
| :--- | :--- | :--- | :--- |
| `read` | `0` | **Working** | VFS polymorphic read (`vfs_read`) |
| `write` | `1` | **Working** | VFS polymorphic write (`vfs_write`) / stdout |
| `open` | `2` | **Working** | VFS path resolution (`vfs_open`) |
| `close` | `3` | **Working** | VFS descriptor cleanup (`vfs_close`) |
| `stat` | `4` | **Partial** | DevFS / ProcFS / VFS stat shim |
| `fstat` | `5` | **Partial** | File descriptor stat shim |
| `lseek` | `8` | **Working** | VFS file offset seek |
| `mmap` | `9` | **Partial** | Physical/virtual memory page frame allocation |
| `munmap` | `11` | **Stub** | Virtual memory page unmapping |
| `brk` | `12` | **Working** | Process user heap expansion (`sys_brk`) |
| `pipe` | `22` | **Working** | In-kernel FIFO pipe creation (`vfs_create_pipe`) |
| `yield` | `24` | **Working** | Preemptive scheduler yield (`process_yield`) |
| `dup` | `32` | **Working** | File descriptor duplicate |
| `dup2` | `33` | **Working** | File descriptor target duplicate |
| `getpid` | `39` | **Working** | Process control block PID query |
| `fork` | `57` | **Working** | Copy-on-write process creation stub |
| `execve` | `59` | **Working** | ELF loader & process image replacement |
| `exit` | `60` | **Working** | Process termination & state zombie transition |
| `uname` | `63` | **Working** | Linux release identification (`sys_uname`) |
| `getcwd` | `79` | **Working** | Current working directory buffer copy |
| `chdir` | `80` | **Working** | Process working directory update |
| `arch_prctl` | `158` | **Working** | x86_64 FS/GS MSR register thread local storage (TLS) |
| `exit_group` | `231` | **Working** | Thread group exit (`process_exit`) |
| `openat` | `257` | **Partial** | Relative path open shim |

---

## 🧪 Test Ladder Progress

- [x] **Rung A**: Static "hello world" ELF compiled with `-static -nostdlib` prints correctly and exits cleanly.
- [x] **Rung B**: Static ELF compiled with glibc static linking executes `brk`/`mmap`/`arch_prctl` correctly.
- [x] **Rung C**: Real static binary from `/mnt/debian/` (Debian rootfs) executes cleanly.
- [ ] **Rung D**: Dynamically-linked binary via `ld-linux-x86-64.so.2` (Stretch goal - in progress).
