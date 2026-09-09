#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

// RatanaOS Syscall ABI Numbers
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

typedef struct {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
} utsname_t;

typedef struct {
    uint64_t st_dev;
    uint64_t st_ino;
    uint32_t st_mode;
    uint32_t st_nlink;
    uint32_t st_uid;
    uint32_t st_gid;
    uint64_t st_size;
    uint64_t st_atime;
    uint64_t st_mtime;
    uint64_t st_ctime;
} stat_t;

void syscall_init(void);

#endif // SYSCALL_H
