#include "../../include/physical.h"
#include "../../include/string.h"
#include "../../include/multiboot.h"
#include "../../include/serial.h"
#include "../../include/stdio.h"

#define MAX_PAGES 1048576 // Supports up to 4GB of physical RAM in 4KB frames
static uint32_t page_bitmap[MAX_PAGES / 32];
static size_t total_pages = 0;
static size_t free_pages = 0;

static inline void set_bit(size_t page_idx) {
    page_bitmap[page_idx / 32] |= (1U << (page_idx % 32));
}

static inline void clear_bit(size_t page_idx) {
    page_bitmap[page_idx / 32] &= ~(1U << (page_idx % 32));
}

static inline bool test_bit(size_t page_idx) {
    return (page_bitmap[page_idx / 32] & (1U << (page_idx % 32))) != 0;
}

void phys_init(multiboot_info_t* mbi) {
    total_pages = 0;
    free_pages = 0;
    memset(page_bitmap, 0xFF, sizeof(page_bitmap));

    if (mbi->flags & (1 << 6)) { // MEMORY MAP flag
        multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)(uintptr_t)mbi->mmap_addr;
        while ((uintptr_t)mmap < mbi->mmap_addr + mbi->mmap_length) {
            if (mmap->type == 1) { // MULTIBOOT_MEMORY_AVAILABLE
                uint64_t addr = mmap->addr;
                uint64_t len = mmap->len;
                uint64_t end = addr + len;
                
                if (end / PAGE_SIZE > total_pages && end / PAGE_SIZE <= MAX_PAGES) {
                    total_pages = end / PAGE_SIZE;
                }
                
                for (uint64_t i = addr; i < end; i += PAGE_SIZE) {
                    size_t page_idx = i / PAGE_SIZE;
                    if (page_idx < MAX_PAGES) {
                        // Protect first 4MB (Kernel, VBE, bootloader)
                        if (page_idx >= (20 * 1024 * 1024) / PAGE_SIZE) {
                            clear_bit(page_idx);
                            free_pages++;
                        }
                    }
                }
            }
            mmap = (multiboot_memory_map_t*)((uintptr_t)mmap + mmap->size + sizeof(mmap->size));
        }
        serial_printf("PMM: Parsed multiboot mmap. Total RAM up to %d MB\n", (total_pages * PAGE_SIZE) / (1024 * 1024));
    } else {
        total_pages = (256 * 1024 * 1024) / PAGE_SIZE;
        size_t first_free_page = (4 * 1024 * 1024) / PAGE_SIZE;
        for (size_t i = first_free_page; i < total_pages; i++) {
            clear_bit(i);
            free_pages++;
        }
        serial_printf("PMM: No multiboot mmap found. Defaulting to 256 MB.\n");
    }
}

void* phys_alloc_page(void) {
    for (size_t i = (20 * 1024 * 1024) / PAGE_SIZE; i < total_pages; i++) {
        if (!test_bit(i)) {
            set_bit(i);
            free_pages--;
            return (void*)(uintptr_t)(i * PAGE_SIZE);
        }
    }
    return NULL;
}

void phys_free_page(void* ptr) {
    uintptr_t addr = (uintptr_t)ptr;
    if (addr % PAGE_SIZE != 0) return;
    size_t page_idx = addr / PAGE_SIZE;
    if (page_idx < total_pages && test_bit(page_idx)) {
        clear_bit(page_idx);
        free_pages++;
    }
}

size_t phys_get_free_pages(void) {
    return free_pages;
}

size_t phys_get_total_pages(void) {
    return total_pages;
}
