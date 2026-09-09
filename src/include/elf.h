#ifndef ELF_H
#define ELF_H

#include "types.h"
#include "process.h"
#include "vfs.h"

#define ELF_MAGIC 0x464C457F

typedef struct {
    uint32_t magic;
    uint8_t  bitness;
    uint8_t  endian;
    uint8_t  header_version;
    uint8_t  abi;
    uint64_t padding;
    uint16_t type;
    uint16_t instruction_set;
    uint32_t elf_version;
    uint64_t entry;
    uint64_t program_header_pos;
    uint64_t section_header_pos;
    uint32_t flags;
    uint16_t header_size;
    uint16_t program_header_entry_size;
    uint16_t program_header_entries;
    uint16_t section_header_entry_size;
    uint16_t section_header_entries;
    uint16_t section_header_name_index;
} __attribute__((packed)) elf64_header_t;

typedef struct {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t virtual_addr;
    uint64_t physical_addr;
    uint64_t file_size;
    uint64_t memory_size;
    uint64_t align;
} __attribute__((packed)) elf64_program_header_t;

process_t* elf_load_and_run(vfs_node_t* file, const char* name);
int elf_execve(vfs_node_t* file, trap_frame_t* regs);

#endif
