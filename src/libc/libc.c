#include "include/unistd.h"
#include "include/stdio.h"
#include "include/stdlib.h"
#include "include/string.h"
#include "include/sys/utsname.h"

// -------------------------------------------------------------
// System Call Wrappers
// -------------------------------------------------------------
int64_t read(int fd, void* buf, size_t count) {
    return syscall3(SYS_READ, (int64_t)fd, (int64_t)buf, (int64_t)count);
}

int64_t write(int fd, const void* buf, size_t count) {
    return syscall3(SYS_WRITE, (int64_t)fd, (int64_t)buf, (int64_t)count);
}

int open(const char* path, int flags) {
    return (int)syscall2(SYS_OPEN, (int64_t)path, (int64_t)flags);
}

int close(int fd) {
    return (int)syscall1(SYS_CLOSE, (int64_t)fd);
}

int64_t lseek(int fd, int64_t offset, int whence) {
    return syscall3(SYS_LSEEK, (int64_t)fd, offset, (int64_t)whence);
}

int pipe(int pipefd[2]) {
    return (int)syscall1(SYS_PIPE, (int64_t)pipefd);
}

int dup(int oldfd) {
    return (int)syscall1(SYS_DUP, (int64_t)oldfd);
}

int dup2(int oldfd, int newfd) {
    return (int)syscall2(SYS_DUP2, (int64_t)oldfd, (int64_t)newfd);
}

int64_t fork(void) {
    return syscall0(SYS_FORK);
}

int execve(const char* path, char* const argv[], char* const envp[]) {
    return (int)syscall3(SYS_EXECVE, (int64_t)path, (int64_t)argv, (int64_t)envp);
}

void _exit(int status) {
    syscall1(SYS_EXIT, (int64_t)status);
    while (1);
}

void exit(int status) {
    _exit(status);
}

int64_t getpid(void) {
    return syscall0(SYS_GETPID);
}

int64_t getppid(void) {
    return syscall0(SYS_GETPPID);
}

int64_t waitpid(int64_t pid, int* status, int options) {
    (void)options;
    return syscall2(SYS_WAIT4, pid, (int64_t)status);
}

void yield(void) {
    syscall0(SYS_YIELD);
}

char* getcwd(char* buf, size_t size) {
    int64_t ret = syscall2(SYS_GETCWD, (int64_t)buf, (int64_t)size);
    if (ret == 0 || ret == -1) return NULL;
    return buf;
}

int chdir(const char* path) {
    return (int)syscall1(SYS_CHDIR, (int64_t)path);
}

int uname(struct utsname* buf) {
    return (int)syscall1(SYS_UNAME, (int64_t)buf);
}

// -------------------------------------------------------------
// Memory Allocator (Heap via sys_brk)
// -------------------------------------------------------------
typedef struct header {
    size_t size;
    bool is_free;
    struct header* next;
} header_t;

static header_t* head = NULL;
static header_t* tail = NULL;

static void* sys_sbrk(intptr_t increment) {
    uint64_t current_brk = (uint64_t)syscall1(SYS_BRK, 0);
    if (increment == 0) return (void*)current_brk;

    uint64_t new_brk = current_brk + increment;
    uint64_t ret = (uint64_t)syscall1(SYS_BRK, new_brk);
    if (ret < new_brk) return (void*)-1;
    return (void*)current_brk;
}

void* malloc(size_t size) {
    if (size == 0) return NULL;
    size = (size + 7) & ~7; // 8-byte alignment

    header_t* curr = head;
    while (curr) {
        if (curr->is_free && curr->size >= size) {
            curr->is_free = false;
            return (void*)(curr + 1);
        }
        curr = curr->next;
    }

    size_t total_size = sizeof(header_t) + size;
    void* block = sys_sbrk(total_size);
    if (block == (void*)-1) return NULL;

    header_t* h = (header_t*)block;
    h->size = size;
    h->is_free = false;
    h->next = NULL;

    if (!head) head = h;
    if (tail) tail->next = h;
    tail = h;

    return (void*)(h + 1);
}

void free(void* ptr) {
    if (!ptr) return;
    header_t* h = (header_t*)ptr - 1;
    h->is_free = true;
}

void* calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void* ptr = malloc(total);
    if (ptr) memset(ptr, 0, total);
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) { free(ptr); return NULL; }
    header_t* h = (header_t*)ptr - 1;
    if (h->size >= size) return ptr;

    void* new_ptr = malloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, h->size);
        free(ptr);
    }
    return new_ptr;
}

// -------------------------------------------------------------
// Standard String Functions
// -------------------------------------------------------------
size_t strlen(const char* str) {
    size_t len = 0;
    while (str && str[len]) len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) { s1++; s2++; n--; }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* d = dest;
    while (n && (*d++ = *src++)) n--;
    while (n--) *d++ = '\0';
    return dest;
}

char* strcat(char* dest, const char* src) {
    char* d = dest;
    while (*d) d++;
    while ((*d++ = *src++));
    return dest;
}

char* strncat(char* dest, const char* src, size_t n) {
    char* d = dest;
    while (*d) d++;
    while (n-- && *src) *d++ = *src++;
    *d = '\0';
    return dest;
}

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) return (char*)s;
        s++;
    }
    return (c == 0) ? (char*)s : NULL;
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    for (; *haystack; haystack++) {
        if (*haystack == *needle) {
            const char *h = haystack, *n = needle;
            while (*h && *n && *h == *n) { h++; n++; }
            if (!*n) return (char*)haystack;
        }
    }
    return NULL;
}

void* memcpy(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

void* memset(void* s, int c, size_t n) {
    uint8_t* p = (uint8_t*)s;
    for (size_t i = 0; i < n; i++) p[i] = (uint8_t)c;
    return s;
}

void* memmove(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    if (d < s) {
        for (size_t i = 0; i < n; i++) d[i] = s[i];
    } else if (d > s) {
        for (size_t i = n; i > 0; i--) d[i - 1] = s[i - 1];
    }
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) return p1[i] - p2[i];
    }
    return 0;
}

int atoi(const char* str) {
    int res = 0, sign = 1;
    while (*str == ' ' || *str == '\t') str++;
    if (*str == '-') { sign = -1; str++; }
    else if (*str == '+') str++;
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return sign * res;
}

int abs(int j) {
    return (j < 0) ? -j : j;
}

// -------------------------------------------------------------
// Standard I/O (printf, sprintf, puts, putchar, getchar)
// -------------------------------------------------------------
int putchar(int c) {
    char ch = (char)c;
    write(STDOUT_FILENO, &ch, 1);
    return c;
}

int puts(const char* s) {
    if (!s) return EOF;
    write(STDOUT_FILENO, s, strlen(s));
    char nl = '\n';
    write(STDOUT_FILENO, &nl, 1);
    return 0;
}

int getchar(void) {
    char c;
    if (read(STDIN_FILENO, &c, 1) > 0) return (int)(unsigned char)c;
    return EOF;
}

static void itoa_dec(int64_t val, char* buf) {
    char tmp[32];
    int i = 0;
    bool neg = false;
    if (val < 0) { neg = true; val = -val; }
    if (val == 0) tmp[i++] = '0';
    else {
        while (val > 0) {
            tmp[i++] = '0' + (val % 10);
            val /= 10;
        }
    }
    if (neg) tmp[i++] = '-';
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

static void itoa_hex(uint64_t val, char* buf) {
    char tmp[32];
    int i = 0;
    const char hex_chars[] = "0123456789abcdef";
    if (val == 0) tmp[i++] = '0';
    else {
        while (val > 0) {
            tmp[i++] = hex_chars[val & 0xF];
            val >>= 4;
        }
    }
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

int vsnprintf(char* str, size_t size, const char* format, va_list args) {
    if (!str || size == 0) return 0;
    size_t out_idx = 0;

    for (size_t i = 0; format[i] != '\0' && out_idx < size - 1; i++) {
        if (format[i] == '%') {
            i++;
            if (format[i] == 's') {
                const char* s = va_arg(args, const char*);
                if (!s) s = "(null)";
                while (*s && out_idx < size - 1) str[out_idx++] = *s++;
            } else if (format[i] == 'd' || format[i] == 'i') {
                int64_t val = va_arg(args, int);
                char num_buf[32];
                itoa_dec(val, num_buf);
                const char* p = num_buf;
                while (*p && out_idx < size - 1) str[out_idx++] = *p++;
            } else if (format[i] == 'u') {
                uint64_t val = va_arg(args, unsigned int);
                char num_buf[32];
                itoa_dec(val, num_buf);
                const char* p = num_buf;
                while (*p && out_idx < size - 1) str[out_idx++] = *p++;
            } else if (format[i] == 'x' || format[i] == 'p') {
                uint64_t val = va_arg(args, uint64_t);
                char num_buf[32];
                itoa_hex(val, num_buf);
                const char* p = num_buf;
                while (*p && out_idx < size - 1) str[out_idx++] = *p++;
            } else if (format[i] == 'c') {
                char c = (char)va_arg(args, int);
                if (out_idx < size - 1) str[out_idx++] = c;
            } else if (format[i] == '%') {
                if (out_idx < size - 1) str[out_idx++] = '%';
            }
        } else {
            str[out_idx++] = format[i];
        }
    }
    str[out_idx] = '\0';
    return (int)out_idx;
}

int sprintf(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vsnprintf(str, 4096, format, args);
    va_end(args);
    return ret;
}

int snprintf(char* str, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vsnprintf(str, size, format, args);
    va_end(args);
    return ret;
}

int printf(const char* format, ...) {
    char buf[1024];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    if (len > 0) {
        write(STDOUT_FILENO, buf, (size_t)len);
    }
    return len;
}
