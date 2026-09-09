#ifndef DEBIANFS_H
#define DEBIANFS_H

#include "types.h"
#include "vfs.h"

#define DEBIANFS_MAGIC 0x46424544 // 'DEBF'

typedef struct {
    char magic[4];
    uint32_t version;
    uint32_t num_files;
} __attribute__((packed)) debianfs_header_t;

typedef struct {
    char path[128];
    uint32_t size;
    uint32_t offset;
} __attribute__((packed)) debianfs_entry_t;

void debianfs_init(void);

#endif // DEBIANFS_H
