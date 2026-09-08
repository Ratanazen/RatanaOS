#include "../include/gdt.h"

extern void gdt_flush(uint64_t);

static uint64_t gdt_entries[3];
static gdt_ptr_t gdt_ptr;

void gdt_init(void) {
    gdt_entries[0] = 0; // Null descriptor
    gdt_entries[1] = 0x00209A0000000000ULL; // 64-bit Code (0x08)
    gdt_entries[2] = 0x0000920000000000ULL; // 64-bit Data (0x10)

    gdt_ptr.limit = sizeof(gdt_entries) - 1;
    gdt_ptr.base  = (uint64_t)&gdt_entries;

    gdt_flush((uint64_t)&gdt_ptr);
}
