#include "../../include/debianfs.h"
#include "../../include/vfs.h"
#include "../../include/heap.h"
#include "../../include/string.h"
#include "../../include/stdio.h"
#include "../../include/serial.h"

extern const unsigned char debian_rootfs_img[];
extern const unsigned int debian_rootfs_img_len;

void debianfs_init(void) {
    if (debian_rootfs_img_len < sizeof(debianfs_header_t)) {
        serial_printf("DEBIANFS: Image too small or corrupt\n");
        return;
    }

    const debianfs_header_t* hdr = (const debianfs_header_t*)debian_rootfs_img;
    if (hdr->magic[0] != 'D' || hdr->magic[1] != 'E' || hdr->magic[2] != 'B' || hdr->magic[3] != 'F') {
        serial_printf("DEBIANFS: Invalid magic header\n");
        return;
    }

    uint32_t num_files = hdr->num_files;
    const debianfs_entry_t* entries = (const debianfs_entry_t*)(debian_rootfs_img + sizeof(debianfs_header_t));
    const uint8_t* data_start = (const uint8_t*)(entries + num_files);

    serial_printf("DEBIANFS: Mounting %u files at /mnt/debian...\n", num_files);

    for (uint32_t i = 0; i < num_files; i++) {
        vfs_node_t* node = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
        if (!node) continue;
        memset(node, 0, sizeof(vfs_node_t));

        ksprintf(node->name, "/mnt/debian/%s", entries[i].path);
        node->flags = VFS_FILE;
        node->size = entries[i].size;
        node->data = (uint8_t*)(data_start + entries[i].offset);
        node->fs_type = FS_RAMFS;

        vfs_register_node(node);
        serial_printf("DEBIANFS: Mounted %s (%u bytes)\n", node->name, node->size);
    }
}
