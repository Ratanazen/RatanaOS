#include "../../include/ext2.h"
#include "../../include/ata.h"
#include "../../include/serial.h"
#include "../../include/heap.h"
#include "../../include/string.h"

static struct ext2_superblock* sb = NULL;
static struct ext2_bg_descriptor* bgdt = NULL;
static uint32_t block_size = 0;

static void read_block(uint32_t block_num, uint8_t* buf) {
    uint32_t sectors_per_block = block_size / 512;
    uint32_t lba = block_num * sectors_per_block;
    ata_read_sectors(lba, sectors_per_block, buf);
}

static void read_inode(uint32_t inode_num, struct ext2_inode* inode) {
    uint32_t bg = (inode_num - 1) / sb->inodes_per_group;
    uint32_t index = (inode_num - 1) % sb->inodes_per_group;
    
    uint32_t inode_table_block = bgdt[bg].inode_table;
    uint32_t inodes_per_block = block_size / sb->inode_size;
    
    uint32_t block_num = inode_table_block + (index / inodes_per_block);
    uint32_t block_offset = (index % inodes_per_block) * sb->inode_size;
    
    uint8_t* buf = kmalloc(block_size);
    read_block(block_num, buf);
    
    memcpy(inode, buf + block_offset, sizeof(struct ext2_inode));
    kfree(buf);
}

void ext2_init(void) {
    uint8_t* buf = kmalloc(1024 * 2); // To comfortably read first few sectors
    ata_read_sectors(2, 2, buf); // Superblock is at byte 1024 (LBA 2)
    
    sb = kmalloc(sizeof(struct ext2_superblock));
    memcpy(sb, buf, sizeof(struct ext2_superblock));
    
    if (sb->magic != EXT2_MAGIC) {
        serial_printf("EXT2: Invalid magic: 0x%x\\n", sb->magic);
        kfree(buf);
        return;
    }
    
    block_size = 1024 << sb->log_block_size;
    
    serial_printf("EXT2: Initialized. Inodes: %d, Blocks: %d, Block Size: %d\\n", sb->inodes_count, sb->blocks_count, block_size);
    
    if (sb->rev_level == 0) {
        sb->inode_size = 128; // Standard for rev 0
    }
    
    // Read BGDT
    uint32_t bgdt_block = (block_size == 1024) ? 2 : 1;
    uint8_t* bgdt_buf = kmalloc(block_size);
    read_block(bgdt_block, bgdt_buf);
    
    uint32_t num_groups = (sb->blocks_count + sb->blocks_per_group - 1) / sb->blocks_per_group;
    bgdt = kmalloc(num_groups * sizeof(struct ext2_bg_descriptor));
    memcpy(bgdt, bgdt_buf, num_groups * sizeof(struct ext2_bg_descriptor));
    
    kfree(bgdt_buf);
    kfree(buf);
    
    // Read root inode (2)
    struct ext2_inode root_inode;
    read_inode(2, &root_inode);
    serial_printf("EXT2: Root inode size: %d, blocks: %d\\n", root_inode.size, root_inode.blocks);
    
    // Test: read root dir blocks
    uint8_t* dir_buf = kmalloc(block_size);
    read_block(root_inode.block[0], dir_buf);
    
    uint32_t offset = 0;
    while (offset < root_inode.size) {
        struct ext2_dir_entry* entry = (struct ext2_dir_entry*)(dir_buf + offset);
        if (entry->inode == 0) break;
        
        char name[256];
        memcpy(name, entry->name, entry->name_len);
        name[entry->name_len] = '\0';
        
        serial_printf("EXT2: Found file '%s' (inode %d)\\n", name, entry->inode);
        
        if (strcmp(name, "hello.txt") == 0) {
            struct ext2_inode hello;
            read_inode(entry->inode, &hello);
            
            uint8_t* file_buf = kmalloc(block_size);
            read_block(hello.block[0], file_buf);
            file_buf[hello.size] = '\0';
            serial_printf("EXT2: hello.txt content: %s\\n", file_buf);
            kfree(file_buf);
        }
        
        offset += entry->rec_len;
    }
    
    kfree(dir_buf);
}

int ext2_read(uint32_t inode_num, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (!sb || !bgdt) return -1;
    
    struct ext2_inode inode;
    read_inode(inode_num, &inode);
    
    if (offset >= inode.size) return 0;
    
    uint32_t read_size = size;
    if (offset + size > inode.size) {
        read_size = inode.size - offset;
    }
    
    uint32_t bytes_read = 0;
    uint32_t current_offset = offset;
    
    uint8_t* block_buf = kmalloc(block_size);
    
    while (bytes_read < read_size) {
        uint32_t block_idx = current_offset / block_size;
        uint32_t offset_in_block = current_offset % block_size;
        
        uint32_t block_num = 0;
        if (block_idx < 12) {
            block_num = inode.block[block_idx];
        } else {
            // Singly indirect (ignore doubly/triply for now for simplicity)
            uint32_t* indirect_buf = (uint32_t*)kmalloc(block_size);
            read_block(inode.block[12], (uint8_t*)indirect_buf);
            block_num = indirect_buf[block_idx - 12];
            kfree((uint8_t*)indirect_buf);
        }
        
        if (block_num == 0) {
            memset(buffer + bytes_read, 0, block_size - offset_in_block); // Sparse block
            uint32_t to_copy = block_size - offset_in_block;
            if (to_copy > read_size - bytes_read) to_copy = read_size - bytes_read;
            bytes_read += to_copy;
            current_offset += to_copy;
            continue;
        }
        
        read_block(block_num, block_buf);
        
        uint32_t to_copy = block_size - offset_in_block;
        if (to_copy > read_size - bytes_read) to_copy = read_size - bytes_read;
        
        memcpy(buffer + bytes_read, block_buf + offset_in_block, to_copy);
        
        bytes_read += to_copy;
        current_offset += to_copy;
    }
    
    kfree(block_buf);
    return read_size;
}

uint32_t ext2_lookup(const char* path) {
    if (!sb || !bgdt) return 0;
    
    // Simplistic lookup (only root directory for now, no subdirectories)
    // Ignore leading slash
    if (path[0] == '/') path++;
    
    struct ext2_inode root_inode;
    read_inode(2, &root_inode);
    
    uint8_t* dir_buf = kmalloc(block_size);
    read_block(root_inode.block[0], dir_buf);
    
    uint32_t offset = 0;
    while (offset < root_inode.size) {
        struct ext2_dir_entry* entry = (struct ext2_dir_entry*)(dir_buf + offset);
        if (entry->inode == 0) break;
        
        char name[256];
        memcpy(name, entry->name, entry->name_len);
        name[entry->name_len] = '\0';
        
        if (strcmp(name, path) == 0) {
            uint32_t ino = entry->inode;
            kfree(dir_buf);
            return ino;
        }
        
        offset += entry->rec_len;
    }
    
    kfree(dir_buf);
    return 0; // Not found
}

size_t ext2_get_size(uint32_t inode_num) {
    if (!sb || !bgdt) return 0;
    struct ext2_inode inode;
    read_inode(inode_num, &inode);
    return inode.size;
}
