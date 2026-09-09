#include "../../include/heap.h"
#include "../../include/vfs.h"
#include "../../include/string.h"
#include "../../include/stdio.h"
#include "../../include/serial.h"
#include "../../include/physical.h"
#include "../../include/ext2.h"
#include "../../include/cpuid.h"
#include "../../include/timer.h"
#include "../../include/keyboard.h"

static vfs_node_t* root_fs = NULL;

// -------------------------------------------------------------
// DevFS Operations
// -------------------------------------------------------------
static int dev_null_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)node; (void)offset; (void)size; (void)buffer;
    return 0; // EOF
}

static int dev_null_write(vfs_node_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer) {
    (void)node; (void)offset; (void)buffer;
    return (int)size; // Discard
}

static int dev_zero_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)node; (void)offset;
    if (!buffer) return -1;
    memset(buffer, 0, size);
    return (int)size;
}

static int dev_zero_write(vfs_node_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer) {
    (void)node; (void)offset; (void)buffer;
    return (int)size;
}

static uint32_t rand_seed = 0x52415441;
static int dev_random_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)node; (void)offset;
    if (!buffer) return -1;
    for (uint32_t i = 0; i < size; i++) {
        rand_seed = rand_seed * 1103515245 + 12345;
        buffer[i] = (uint8_t)(rand_seed >> 16);
    }
    return (int)size;
}

static int dev_console_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)node; (void)offset;
    if (!buffer || size == 0) return 0;
    int count = 0;
    while (keyboard_has_key() && (uint32_t)count < size) {
        buffer[count++] = (uint8_t)keyboard_getchar();
    }
    return count;
}

static int dev_console_write(vfs_node_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer) {
    (void)node; (void)offset;
    if (!buffer) return -1;
    for (uint32_t i = 0; i < size; i++) {
        serial_printf("%c", (char)buffer[i]);
    }
    return (int)size;
}

void devfs_init(void) {
    // /dev/null
    vfs_node_t* null_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(null_node, 0, sizeof(vfs_node_t));
    strcpy(null_node->name, "/dev/null");
    null_node->flags = VFS_CHARDEVICE;
    null_node->fs_type = FS_DEVFS;
    null_node->read = dev_null_read;
    null_node->write = dev_null_write;
    vfs_register_node(null_node);

    // /dev/zero
    vfs_node_t* zero_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(zero_node, 0, sizeof(vfs_node_t));
    strcpy(zero_node->name, "/dev/zero");
    zero_node->flags = VFS_CHARDEVICE;
    zero_node->fs_type = FS_DEVFS;
    zero_node->read = dev_zero_read;
    zero_node->write = dev_zero_write;
    vfs_register_node(zero_node);

    // /dev/random & /dev/urandom
    vfs_node_t* rand_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(rand_node, 0, sizeof(vfs_node_t));
    strcpy(rand_node->name, "/dev/random");
    rand_node->flags = VFS_CHARDEVICE;
    rand_node->fs_type = FS_DEVFS;
    rand_node->read = dev_random_read;
    rand_node->write = dev_null_write;
    vfs_register_node(rand_node);

    vfs_node_t* urand_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(urand_node, 0, sizeof(vfs_node_t));
    strcpy(urand_node->name, "/dev/urandom");
    urand_node->flags = VFS_CHARDEVICE;
    urand_node->fs_type = FS_DEVFS;
    urand_node->read = dev_random_read;
    urand_node->write = dev_null_write;
    vfs_register_node(urand_node);

    // /dev/console & /dev/tty
    vfs_node_t* console_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(console_node, 0, sizeof(vfs_node_t));
    strcpy(console_node->name, "/dev/console");
    console_node->flags = VFS_CHARDEVICE;
    console_node->fs_type = FS_DEVFS;
    console_node->read = dev_console_read;
    console_node->write = dev_console_write;
    vfs_register_node(console_node);

    vfs_node_t* tty_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(tty_node, 0, sizeof(vfs_node_t));
    strcpy(tty_node->name, "/dev/tty");
    tty_node->flags = VFS_CHARDEVICE;
    tty_node->fs_type = FS_DEVFS;
    tty_node->read = dev_console_read;
    tty_node->write = dev_console_write;
    vfs_register_node(tty_node);

    serial_printf("DEVFS: /dev/null, /dev/zero, /dev/random, /dev/console initialized\n");
}

// -------------------------------------------------------------
// ProcFS Operations
// -------------------------------------------------------------
static int proc_cpuinfo_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)node;
    if (!buffer) return -1;
    cpu_info_t cpu = cpuid_get_info();
    char text[512];
    ksprintf(text, "processor\t: 0\nvendor_id\t: %s\nmodel name\t: %s\narchitecture\t: x86_64 (64-bit Long Mode)\nfeatures\t: sse sse2 sse3 avx apic tsc nx lm\n",
             cpu.vendor, cpu.brand);
    uint32_t len = strlen(text);
    if (offset >= len) return 0;
    uint32_t to_copy = size;
    if (offset + to_copy > len) to_copy = len - offset;
    memcpy(buffer, text + offset, to_copy);
    return (int)to_copy;
}

static int proc_meminfo_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)node;
    if (!buffer) return -1;
    char text[256];
    ksprintf(text, "MemTotal:\t262144 kB\nMemFree:\t245760 kB\nHeapTotal:\t16384 kB\nHeapFree:\t14336 kB\nPageSize:\t4 kB\n");
    uint32_t len = strlen(text);
    if (offset >= len) return 0;
    uint32_t to_copy = size;
    if (offset + to_copy > len) to_copy = len - offset;
    memcpy(buffer, text + offset, to_copy);
    return (int)to_copy;
}

static int proc_uptime_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)node;
    if (!buffer) return -1;
    char text[64];
    uint32_t ticks = timer_get_ticks();
    uint32_t seconds = ticks / 100;
    ksprintf(text, "%u.%02u seconds\n", seconds, ticks % 100);
    uint32_t len = strlen(text);
    if (offset >= len) return 0;
    uint32_t to_copy = size;
    if (offset + to_copy > len) to_copy = len - offset;
    memcpy(buffer, text + offset, to_copy);
    return (int)to_copy;
}

static int proc_version_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)node;
    if (!buffer) return -1;
    const char* ver = "RatanaOS version 1.0.0-macos-sequoia (x86_64 freestanding kernel, build 2026.09)\n";
    uint32_t len = strlen(ver);
    if (offset >= len) return 0;
    uint32_t to_copy = size;
    if (offset + to_copy > len) to_copy = len - offset;
    memcpy(buffer, ver + offset, to_copy);
    return (int)to_copy;
}

void procfs_init(void) {
    // /proc/cpuinfo
    vfs_node_t* cpu_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(cpu_node, 0, sizeof(vfs_node_t));
    strcpy(cpu_node->name, "/proc/cpuinfo");
    cpu_node->flags = VFS_FILE;
    cpu_node->fs_type = FS_PROCFS;
    cpu_node->read = proc_cpuinfo_read;
    vfs_register_node(cpu_node);

    // /proc/meminfo
    vfs_node_t* mem_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(mem_node, 0, sizeof(vfs_node_t));
    strcpy(mem_node->name, "/proc/meminfo");
    mem_node->flags = VFS_FILE;
    mem_node->fs_type = FS_PROCFS;
    mem_node->read = proc_meminfo_read;
    vfs_register_node(mem_node);

    // /proc/uptime
    vfs_node_t* up_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(up_node, 0, sizeof(vfs_node_t));
    strcpy(up_node->name, "/proc/uptime");
    up_node->flags = VFS_FILE;
    up_node->fs_type = FS_PROCFS;
    up_node->read = proc_uptime_read;
    vfs_register_node(up_node);

    // /proc/version
    vfs_node_t* ver_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(ver_node, 0, sizeof(vfs_node_t));
    strcpy(ver_node->name, "/proc/version");
    ver_node->flags = VFS_FILE;
    ver_node->fs_type = FS_PROCFS;
    ver_node->read = proc_version_read;
    vfs_register_node(ver_node);

    serial_printf("PROCFS: /proc/cpuinfo, /proc/meminfo, /proc/uptime, /proc/version initialized\n");
}

// -------------------------------------------------------------
// Pipe IPC Subsystem
// -------------------------------------------------------------
#define PIPE_BUFFER_SIZE 4096

typedef struct pipe_buffer {
    uint8_t buffer[PIPE_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    bool write_closed;
    bool read_closed;
} pipe_buffer_t;

static int pipe_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)offset;
    if (!node || !node->internal_ptr || !buffer || size == 0) return -1;
    pipe_buffer_t* p = (pipe_buffer_t*)node->internal_ptr;

    uint32_t bytes_read = 0;
    while (bytes_read < size && p->count > 0) {
        buffer[bytes_read++] = p->buffer[p->tail];
        p->tail = (p->tail + 1) % PIPE_BUFFER_SIZE;
        p->count--;
    }
    return (int)bytes_read;
}

static int pipe_write(vfs_node_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer) {
    (void)offset;
    if (!node || !node->internal_ptr || !buffer) return -1;
    pipe_buffer_t* p = (pipe_buffer_t*)node->internal_ptr;
    if (p->read_closed) return -1; // Broken pipe

    uint32_t bytes_written = 0;
    while (bytes_written < size && p->count < PIPE_BUFFER_SIZE) {
        p->buffer[p->head] = buffer[bytes_written++];
        p->head = (p->head + 1) % PIPE_BUFFER_SIZE;
        p->count++;
    }
    return (int)bytes_written;
}

static void pipe_close_read(vfs_node_t* node) {
    if (node && node->internal_ptr) {
        pipe_buffer_t* p = (pipe_buffer_t*)node->internal_ptr;
        p->read_closed = true;
        if (p->write_closed) {
            kfree(p);
        }
        kfree(node);
    }
}

static void pipe_close_write(vfs_node_t* node) {
    if (node && node->internal_ptr) {
        pipe_buffer_t* p = (pipe_buffer_t*)node->internal_ptr;
        p->write_closed = true;
        if (p->read_closed) {
            kfree(p);
        }
        kfree(node);
    }
}

int vfs_create_pipe(vfs_node_t** read_end, vfs_node_t** write_end) {
    if (!read_end || !write_end) return -1;

    pipe_buffer_t* p = (pipe_buffer_t*)kmalloc(sizeof(pipe_buffer_t));
    if (!p) return -1;
    memset(p, 0, sizeof(pipe_buffer_t));

    vfs_node_t* r_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(r_node, 0, sizeof(vfs_node_t));
    strcpy(r_node->name, "[pipe:read]");
    r_node->flags = VFS_PIPE;
    r_node->fs_type = FS_PIPE;
    r_node->read = pipe_read;
    r_node->close = pipe_close_read;
    r_node->internal_ptr = p;

    vfs_node_t* w_node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    memset(w_node, 0, sizeof(vfs_node_t));
    strcpy(w_node->name, "[pipe:write]");
    w_node->flags = VFS_PIPE;
    w_node->fs_type = FS_PIPE;
    w_node->write = pipe_write;
    w_node->close = pipe_close_write;
    w_node->internal_ptr = p;

    *read_end = r_node;
    *write_end = w_node;
    return 0;
}

// -------------------------------------------------------------
// Core VFS Dispatcher
// -------------------------------------------------------------
void vfs_init(void) {
    root_fs = NULL;
    devfs_init();
    procfs_init();
    serial_printf("VFS: Virtual File System initialized\n");
}

void vfs_register_node(vfs_node_t* node) {
    if (!node) return;
    node->next = root_fs;
    root_fs = node;
}

vfs_node_t* vfs_open(const char* path) {
    if ((uint64_t)path < 0x1000) return 0;
    if (!path) return NULL;
    vfs_node_t* curr = root_fs;
    while (curr) {
        if (strcmp(curr->name, path) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    
    // Fallback to EXT2 lookup
    uint32_t ext2_ino = ext2_lookup(path);
    if (ext2_ino > 0) {
        vfs_node_t* node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
        memset(node, 0, sizeof(vfs_node_t));
        strcpy(node->name, path);
        node->size = ext2_get_size(ext2_ino);
        node->inode = ext2_ino;
        node->fs_type = FS_EXT2;
        node->flags = VFS_FILE;
        node->data = NULL;
        return node;
    }

    // Fallback to /system/debian alias for Debian rootfs transparent resolution
    if (path[0] == '/' && strncmp(path, "/system/debian", 14) != 0) {
        char debian_path[256];
        ksprintf(debian_path, "/system/debian%s", path);
        curr = root_fs;
        while (curr) {
            if (strcmp(curr->name, debian_path) == 0) {
                return curr;
            }
            curr = curr->next;
        }
    }
    
    return NULL;
}

int vfs_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!node || !buffer) return -1;
    
    // 1. Check polymorphic node handler
    if (node->read) {
        return node->read(node, offset, size, buffer);
    }

    // 2. EXT2 filesystem
    if (node->fs_type == FS_EXT2) {
        return ext2_read(node->inode, offset, size, buffer);
    }
    
    // 3. RAMFS buffer
    if (!node->data) return -1;
    if (offset >= node->size) return 0;
    
    uint32_t read_size = size;
    if (offset + size > node->size) {
        read_size = node->size - offset;
    }
    
    memcpy(buffer, node->data + offset, read_size);
    return (int)read_size;
}

vfs_node_t* vfs_create_file(const char* path, uint32_t flags) {
    if (!path) return NULL;
    vfs_node_t* existing = vfs_open(path);
    if (existing) return existing;

    vfs_node_t* node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    if (!node) return NULL;
    memset(node, 0, sizeof(vfs_node_t));
    strcpy(node->name, path);
    node->flags = flags ? flags : VFS_FILE;
    node->fs_type = FS_RAMFS;
    node->size = 0;
    node->data = (uint8_t*)kmalloc(4096);
    if (node->data) memset(node->data, 0, 4096);
    vfs_register_node(node);
    return node;
}

int vfs_write(vfs_node_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer) {
    if (!node || !buffer) return -1;
    if (node->write) {
        return node->write(node, offset, size, buffer);
    }
    if (node->fs_type == FS_RAMFS) {
        if (!node->data) {
            node->data = (uint8_t*)kmalloc(4096);
            if (node->data) memset(node->data, 0, 4096);
        }
        if (!node->data) return -1;
        uint32_t max_write = 4096 - offset;
        if (size > max_write) size = max_write;
        memcpy(node->data + offset, buffer, size);
        if (offset + size > node->size) {
            node->size = offset + size;
        }
        return (int)size;
    }
    return -1; // Read-only for standard files
}

void vfs_close(vfs_node_t* node) {
    if (!node) return;
    if (node->close) {
        node->close(node);
    } else if (node->fs_type == FS_EXT2) {
        kfree(node);
    }
}

