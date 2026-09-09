#ifndef VFS_H
#define VFS_H

#include "types.h"

#define VFS_MAX_PATH 256

#define VFS_FILE        0x01
#define VFS_DIRECTORY   0x02
#define VFS_CHARDEVICE  0x03
#define VFS_BLOCKDEVICE 0x04
#define VFS_PIPE        0x05

#define FS_RAMFS 0
#define FS_EXT2  1
#define FS_DEVFS 2
#define FS_PROCFS 3
#define FS_PIPE  4

struct vfs_node;

typedef int (*vfs_read_type_t)(struct vfs_node* node, uint32_t offset, uint32_t size, uint8_t* buffer);
typedef int (*vfs_write_type_t)(struct vfs_node* node, uint32_t offset, uint32_t size, const uint8_t* buffer);
typedef void (*vfs_open_type_t)(struct vfs_node* node);
typedef void (*vfs_close_type_t)(struct vfs_node* node);

typedef struct vfs_node {
    char name[64];
    uint32_t flags;
    uint32_t size;
    uint8_t* data;
    uint32_t inode;
    uint32_t fs_type;
    uint32_t ref_count;
    
    vfs_read_type_t  read;
    vfs_write_type_t write;
    vfs_open_type_t  open;
    vfs_close_type_t close;
    
    void* internal_ptr;
    struct vfs_node* next;
} vfs_node_t;

void vfs_init(void);
void vfs_register_node(vfs_node_t* node);
vfs_node_t* vfs_open(const char* path);
int vfs_read(vfs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
int vfs_write(vfs_node_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer);
void vfs_close(vfs_node_t* node);

// Device Filesystem & ProcFS
void devfs_init(void);
void procfs_init(void);

// In-kernel Pipe IPC
int vfs_create_pipe(vfs_node_t** read_end, vfs_node_t** write_end);

// Initramfs
void initramfs_init(void);

#endif
