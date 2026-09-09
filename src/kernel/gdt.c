#include "../include/gdt.h"
#include "../include/string.h"

extern void gdt_flush(uint64_t);
extern void tss_flush(void);

static uint64_t gdt_entries[7];
static gdt_ptr_t gdt_ptr;
static tss_entry_t tss_entry;

static void gdt_set_entry(int index, uint64_t value) {
    gdt_entries[index] = value;
}

static void write_tss_descriptor(int index, uint64_t base, uint32_t limit) {
    uint64_t descriptor_low = 0;
    uint64_t descriptor_high = 0;
    
    descriptor_low |= (limit & 0xFFFF);
    descriptor_low |= ((base & 0xFFFFFF) << 16);
    descriptor_low |= (0x89ULL << 40); // Present, Ring 0, 64-bit TSS (Available)
    descriptor_low |= (((uint64_t)limit & 0xF0000) << 32);
    descriptor_low |= (((base >> 24) & 0xFF) << 56);
    
    descriptor_high = (base >> 32) & 0xFFFFFFFF;
    
    gdt_entries[index] = descriptor_low;
    gdt_entries[index + 1] = descriptor_high;
}

void gdt_init(void) {
    gdt_set_entry(0, 0); // Null descriptor
    gdt_set_entry(1, 0x00209A0000000000ULL); // 0x08: 64-bit Kernel Code
    gdt_set_entry(2, 0x0000920000000000ULL); // 0x10: 64-bit Kernel Data
    gdt_set_entry(3, 0x0020FA0000000000ULL); // 0x18: 64-bit User Code (Ring 3)
    gdt_set_entry(4, 0x0000F20000000000ULL); // 0x20: 64-bit User Data (Ring 3)
    
    memset(&tss_entry, 0, sizeof(tss_entry_t));
    tss_entry.iopb_offset = sizeof(tss_entry_t);
    
    write_tss_descriptor(5, (uint64_t)&tss_entry, sizeof(tss_entry_t) - 1);
    
    gdt_ptr.limit = sizeof(gdt_entries) - 1;
    gdt_ptr.base  = (uint64_t)&gdt_entries;

    gdt_flush((uint64_t)&gdt_ptr);
    tss_flush();
}

void tss_set_kernel_stack(uint64_t stack) {
    tss_entry.rsp0 = stack;
}
