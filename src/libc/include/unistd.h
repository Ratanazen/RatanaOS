#ifndef _UNISTD_H
#define _UNISTD_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define SYS_READ      0
#define SYS_WRITE     1
#define SYS_OPEN      2
#define SYS_CLOSE     3
#define SYS_STAT      4
#define SYS_FSTAT     5
#define SYS_LSEEK     8
#define SYS_MMAP      9
#define SYS_MUNMAP    11
#define SYS_BRK       12
#define SYS_IOCTL     16
#define SYS_PIPE      22
#define SYS_YIELD     24
#define SYS_DUP       32
#define SYS_DUP2      33
#define SYS_GETPID    39
#define SYS_FORK      57
#define SYS_EXECVE    59
#define SYS_EXIT      60
#define SYS_WAIT4     61
#define SYS_UNAME     63
#define SYS_GETCWD    79
#define SYS_CHDIR     80
#define SYS_MKDIR     83
#define SYS_UNLINK    87
#define SYS_GETPPID   110

static inline int64_t syscall0(int64_t num) {
    int64_t ret;
    __asm__ volatile ("int $0x80" : "=a"(ret) : "a"(num) : "rcx", "r11", "memory");
    return ret;
}

static inline int64_t syscall1(int64_t num, int64_t a1) {
    int64_t ret;
    __asm__ volatile ("int $0x80" : "=a"(ret) : "a"(num), "D"(a1) : "rcx", "r11", "memory");
    return ret;
}

static inline int64_t syscall2(int64_t num, int64_t a1, int64_t a2) {
    int64_t ret;
    __asm__ volatile ("int $0x80" : "=a"(ret) : "a"(num), "D"(a1), "S"(a2) : "rcx", "r11", "memory");
    return ret;
}

static inline int64_t syscall3(int64_t num, int64_t a1, int64_t a2, int64_t a3) {
    int64_t ret;
    __asm__ volatile ("int $0x80" : "=a"(ret) : "a"(num), "D"(a1), "S"(a2), "d"(a3) : "rcx", "r11", "memory");
    return ret;
}

int64_t read(int fd, void* buf, size_t count);
int64_t write(int fd, const void* buf, size_t count);
int open(const char* path, int flags);
int close(int fd);
int64_t lseek(int fd, int64_t offset, int whence);
int pipe(int pipefd[2]);
int dup(int oldfd);
int dup2(int oldfd, int newfd);
int64_t fork(void);
int execve(const char* path, char* const argv[], char* const envp[]);
void _exit(int status);
int64_t getpid(void);
int64_t getppid(void);
int64_t waitpid(int64_t pid, int* status, int options);
void yield(void);
char* getcwd(char* buf, size_t size);
int chdir(const char* path);

#endif // _UNISTD_H
