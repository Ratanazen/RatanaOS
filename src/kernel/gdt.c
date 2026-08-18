#include "../include/gdt.h"

extern void gdt_flush(uint64_t);

static uint64_t gdt_entries[3];
static gdt_ptr_t gdt_ptr;

void gdt_init(void) {
    gdt_entries[0] = 0; // Null descriptor
    // 64-bit Kernel Code Segment (0x08): Present, Ring 0, Code, Exec/Read, Long Mode (L=1)
    gdt_entries[1] = 0x00AF9A000000FFFFULL;
    // 64-bit Kernel Data Segment (0x10): Present, Ring 0, Data, Read/Write
    gdt_entries[2] = 0x00AF92000000FFFFULL;

    gdt_ptr.limit = sizeof(gdt_entries) - 1;
    gdt_ptr.base  = (uint64_t)&gdt_entries;

    gdt_flush((uint64_t)&gdt_ptr);
}
