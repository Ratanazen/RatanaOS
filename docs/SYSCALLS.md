# 📜 RatanaOS Syscall ABI Specification

The RatanaOS kernel implements a custom 64-bit System Call Application Binary Interface (ABI) invoked via software interrupt `int $0x80`.

---

## Syscall Calling Convention (x86_64)

- **Syscall Number**: `%rax`
- **Argument 1**: `%rdi`
- **Argument 2**: `%rsi`
- **Argument 3**: `%rdx`
- **Argument 4**: `%r10`
- **Argument 5**: `%r8`
- **Argument 6**: `%r9`
- **Return Value**: `%rax` (positive on success or byte count, negative `-errno` on failure)

---

## Syscall Table

| Number | Name | Arguments | Description |
|---|---|---|---|
| `0` | `read` | `int fd, void* buf, size_t count` | Read bytes from a file descriptor |
| `1` | `write` | `int fd, const void* buf, size_t count` | Write bytes to a file descriptor |
| `2` | `open` | `const char* path, int flags` | Open or create a file / device node |
| `3` | `close` | `int fd` | Close an active file descriptor |
| `4` | `stat` | `const char* path, struct stat* buf` | Get file attributes by path |
| `5` | `fstat` | `int fd, struct stat* buf` | Get file attributes by descriptor |
| `8` | `lseek` | `int fd, int64_t offset, int whence` | Reposition read/write file offset |
| `9` | `mmap` | `void* addr, size_t len, int prot, int flags, int fd, int64_t off` | Map pages into address space |
| `11` | `munmap` | `void* addr, size_t len` | Unmap pages from address space |
| `12` | `brk` | `void* addr` | Change heap data segment end address |
| `16` | `ioctl` | `int fd, unsigned long req, void* arg` | Device control operations |
| `22` | `pipe` | `int pipefd[2]` | Create an in-kernel unidirectional IPC pipe |
| `24` | `yield` | `void` | Yield current CPU timeslice to scheduler |
| `32` | `dup` | `int oldfd` | Duplicate an open file descriptor |
| `33` | `dup2` | `int oldfd, int newfd` | Duplicate a descriptor to a target slot |
| `39` | `getpid` | `void` | Get current process ID |
| `57` | `fork` | `void` | Clone process state and address space |
| `59` | `execve` | `const char* path, char* const argv[], char* const envp[]` | Execute an ELF64 binary in address space |
| `60` | `exit` | `int status` | Terminate the calling process |
| `61` | `wait4` | `int64_t pid, int* status, int options, void* rusage` | Wait for child process termination |
| `63` | `uname` | `struct utsname* buf` | Retrieve system name and kernel version |
| `79` | `getcwd` | `char* buf, size_t size` | Get current working directory path |
| `80` | `chdir` | `const char* path` | Change current working directory |
| `83` | `mkdir` | `const char* path, int mode` | Create a directory node |
| `87` | `unlink` | `const char* path` | Delete a filesystem entry |
| `110` | `getppid` | `void` | Get parent process ID |

---

## Userspace Integration

All syscalls are wrapped cleanly in `src/libc/include/unistd.h` and implemented in `src/libc/libc.c`. Userspace applications do not require raw inline assembly.
