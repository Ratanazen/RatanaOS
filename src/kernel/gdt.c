#include "../include/gdt.h"

extern void gdt_flush(uint32_t);

static gdt_entry_t gdt_entries[5];
static gdt_ptr_t   gdt_ptr;

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

void gdt_init(void) {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    // Null descriptor
    gdt_set_gate(0, 0, 0, 0, 0);
    // Kernel Code Segment: Base 0, Limit 4GB, Ring 0, Execute/Read
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    // Kernel Data Segment: Base 0, Limit 4GB, Ring 0, Read/Write
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    // User Code Segment: Base 0, Limit 4GB, Ring 3, Execute/Read
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    // User Data Segment: Base 0, Limit 4GB, Ring 3, Read/Write
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt_flush((uint32_t)&gdt_ptr);
}
