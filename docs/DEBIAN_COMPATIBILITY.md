# 📦 RatanaOS Debian Compatibility & Package Ecosystem

RatanaOS provides dual packaging and compatibility pathways:
1. **Native RatanaOS Packages (`.rpk`)**: Lightweight native binary archives managed by `ratapkg`.
2. **Debian Package Import & Compatibility Layer (`.deb`)**: Static and dynamic binary analysis via `debimport` evaluating Debian packages against RatanaOS POSIX ABI shims.

---

## Compatibility Classification Matrix

| Subsystem / ABI | Compatibility Level | Notes |
|---|---|---|
| **ELF64 Format** | `[PASS]` | Native support for standard System V x86-64 ELF executables |
| **Basic Syscalls** | `[PASS]` | `read`, `write`, `open`, `close`, `stat`, `fstat`, `lseek`, `brk`, `getpid`, `fork`, `execve`, `exit` |
| **Process Control** | `[PASS]` | Preemptive scheduling, signals groundwork, PID/PPID hierarchy |
| **Standard File I/O** | `[PASS]` | VFS device abstraction, DevFS (`/dev/console`, `/dev/null`), ProcFS |
| **IPC** | `[PASS]` | Unidirectional in-kernel FIFO pipes (`pipe`, `dup2`) |
| **Dynamic Linking** | `[PARTIAL]` | Requires static compilation or `ld-linux` ELF interpreter shim |
| **Terminal / NCurses** | `[PORT REQUIRED]` | Requires terminal capability mapping or ANSI VT100 translations |
| **Direct Hardware Access** | `[UNSUPPORTED]` | Linux `/sys/devices` or direct kernel module loading is not supported |

---

## Package Tools

### Native Package Manager (`/bin/ratapkg`)
```bash
ratapkg list
ratapkg install textedit.rpk
ratapkg search editor
ratapkg info ratana-gui
```

### Debian Package Analyzer (`/bin/debimport`)
```bash
debimport nano_7.2-1_amd64.deb
```
Outputs AR member decomposition, metadata parsing, and ABI dependency validation.
