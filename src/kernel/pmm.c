#include "../include/pmm.h"
#include "../include/string.h"

#define MAX_PAGES 32768 // Supports up to 128MB of physical RAM
static uint32_t page_bitmap[MAX_PAGES / 32];
static size_t total_pages = 0;
static size_t free_pages = 0;

static inline void set_bit(size_t page_idx) {
    page_bitmap[page_idx / 32] |= (1 << (page_idx % 32));
}

static inline void clear_bit(size_t page_idx) {
    page_bitmap[page_idx / 32] &= ~(1 << (page_idx % 32));
}

static inline bool test_bit(size_t page_idx) {
    return (page_bitmap[page_idx / 32] & (1 << (page_idx % 32))) != 0;
}

void pmm_init(uint32_t mem_size_bytes) {
    total_pages = mem_size_bytes / PAGE_SIZE;
    if (total_pages > MAX_PAGES) {
        total_pages = MAX_PAGES;
    }

    memset(page_bitmap, 0xFF, sizeof(page_bitmap)); // Mark all as used initially
    free_pages = 0;

    // Mark pages above 4MB as free (first 4MB reserved for kernel, stack, BIOS, VGA, DMA)
    size_t first_free_page = (4 * 1024 * 1024) / PAGE_SIZE;
    for (size_t i = first_free_page; i < total_pages; i++) {
        clear_bit(i);
        free_pages++;
    }
}

void* pmm_alloc_page(void) {
    for (size_t i = (4 * 1024 * 1024) / PAGE_SIZE; i < total_pages; i++) {
        if (!test_bit(i)) {
            set_bit(i);
            free_pages--;
            return (void*)(i * PAGE_SIZE);
        }
    }
    return NULL; // Out of physical memory
}

void pmm_free_page(void* ptr) {
    uintptr_t addr = (uintptr_t)ptr;
    size_t page_idx = addr / PAGE_SIZE;
    if (page_idx < total_pages && test_bit(page_idx)) {
        clear_bit(page_idx);
        free_pages++;
    }
}

size_t pmm_get_free_pages(void) {
    return free_pages;
}

size_t pmm_get_total_pages(void) {
    return total_pages;
}
