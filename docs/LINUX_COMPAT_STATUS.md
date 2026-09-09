# 🐧 Linux Syscall Compatibility Status (Track C)

This document tracks the implementation status of Linux x86_64 ABI and system call compatibility in the RatanaOS kernel.

---

## 📊 Linux Syscall Status Table

| Linux Syscall Name | Linux Syscall # (x86_64) | Implementation Status | Routing Subsystem / Notes |
| :--- | :--- | :--- | :--- |
| `read` | `0` | **Working** | VFS polymorphic read (`vfs_read`) / stdin |
| `write` | `1` | **Working** | VFS polymorphic write (`vfs_write`) / stdout / serial |
| `open` | `2` | **Working** | VFS path resolution with `/system/debian` fallback (`vfs_open`) |
| `close` | `3` | **Working** | VFS descriptor cleanup (`vfs_close`) |
| `stat` | `4` | **Working** | DevFS / ProcFS / VFS stat shim |
| `fstat` | `5` | **Working** | File descriptor stat shim |
| `lseek` | `8` | **Working** | VFS file offset seek |
| `mmap` | `9` | **Working** | Dynamic anonymous & file-backed VMM page allocation (`do_mmap`) |
| `mprotect` | `10` | **Stub** | Memory protection shim (returns 0) |
| `munmap` | `11` | **Stub** | Virtual memory page unmapping shim (returns 0) |
| `brk` | `12` | **Working** | Process user heap dynamic expansion (`sys_brk`) |
| `rt_sigaction` | `13` | **Stub** | Linux signal handler registration stub |
| `rt_sigprocmask` | `14` | **Stub** | Linux signal mask stub |
| `ioctl` | `16` | **Working** | Terminal ioctl shim (`TCGETS`, `TIOCGWINSZ`) |
| `access` | `21` | **Working** | File access permission check via `vfs_open` |
| `pipe` | `22` | **Working** | In-kernel FIFO pipe creation (`vfs_create_pipe`) |
| `sched_yield` | `24` | **Working** | Preemptive scheduler yield (`process_yield`) |
| `dup` | `32` | **Working** | File descriptor duplicate |
| `dup2` | `33` | **Working** | File descriptor target duplicate |
| `getpid` | `39` | **Working** | Process control block PID query |
| `clone` | `56` | **Working** | Thread/Process fork via `process_fork` |
| `fork` | `57` | **Working** | Isolated PML4 memory copy & PCB fork (`process_fork`) |
| `execve` | `59` | **Working** | Address space replacement & ELF loader (`elf_execve`) |
| `exit` | `60` | **Working** | Process termination & state zombie transition |
| `wait4` | `61` | **Working** | Child process wait and reap (`sys_wait4`) |
| `uname` | `63` | **Working** | Linux release identification (`sys_uname`) |
| `fcntl` | `72` | **Working** | File control flags shim |
| `getcwd` | `79` | **Working** | Current working directory buffer copy |
| `chdir` | `80` | **Working** | Process working directory update |
| `mkdir` | `83` | **Working** | Directory creation shim |
| `unlink` | `87` | **Working** | File removal shim |
| `readlink` | `89` | **Working** | Symlink / `/proc/self/exe` resolution |
| `gettimeofday` | `96` | **Working** | RTC clock time query |
| `getuid` | `102` | **Working** | Root user identity |
| `getgid` | `104` | **Working** | Root group identity |
| `geteuid` | `107` | **Working** | Effective root user identity |
| `getegid` | `108` | **Working** | Effective root group identity |
| `getppid` | `110` | **Working** | Parent process PID query |
| `arch_prctl` | `158` | **Working** | x86_64 FS/GS MSR register thread local storage (TLS) |
| `set_tid_address` | `218` | **Working** | Thread ID address registration |
| `clock_gettime` | `228` | **Working** | Monotonic & Realtime clock query |
| `exit_group` | `231` | **Working** | Thread group exit (`process_exit`) |
| `openat` | `257` | **Working** | Relative directory path open |
| `newfstatat` | `262` | **Working** | Directory-relative file stat shim |
| `prlimit64` | `302` | **Working** | Process resource limits shim |
| `getrandom` | `318` | **Working** | Entropy / random byte generator |

---

## 🧪 Test Ladder Progress

- [x] **Level 0**: Native RatanaOS freestanding kernel, VFS, and GUI.
- [x] **Level 1**: Debian Dual Boot & ISO integration.
- [x] **Level 2**: Debian RootFS mounted at `/system/debian` and `/mnt/debian`.
- [x] **Level 3**: Linux static ELF executable (write + exit) prints and terminates cleanly.
- [x] **Level 4**: Static Debian binary executable runs via Linux SYSCALL MSR gate.
- [x] **Level 5**: Dynamic Linux ELF executable (PT_LOAD and stack initialization).
- [x] **Level 6**: glibc-compatible Debian application environment (TLS via `arch_prctl`, `mmap`, `brk`, `set_tid_address`).
- [x] **Level 7**: Debian shell & coreutils binary execution via `elf_execve` and `/system/debian` path translation.
- [x] **Level 8**: Debian Userspace integration into Terminal and Finder.

