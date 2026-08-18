#include "../include/heap.h"
#include "../include/string.h"

typedef struct block_header {
    size_t size;                // Size of data area
    bool is_free;               // True if block is free
    struct block_header* next;  // Next block in heap
} block_header_t;

#define HEADER_SIZE sizeof(block_header_t)

static block_header_t* heap_start = NULL;
static size_t heap_total_size = 0;

void heap_init(uint32_t start_addr, size_t size) {
    heap_start = (block_header_t*)start_addr;
    heap_total_size = size;

    heap_start->size = size - HEADER_SIZE;
    heap_start->is_free = true;
    heap_start->next = NULL;
}

void* kmalloc(size_t size) {
    if (size == 0) return NULL;

    // 8-byte alignment
    size = (size + 7) & ~7;

    block_header_t* current = heap_start;
    while (current) {
        if (current->is_free && current->size >= size) {
            // Check if we can split the block
            if (current->size >= size + HEADER_SIZE + 16) {
                block_header_t* new_block = (block_header_t*)((uint8_t*)current + HEADER_SIZE + size);
                new_block->size = current->size - size - HEADER_SIZE;
                new_block->is_free = true;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }
            current->is_free = false;
            return (void*)((uint8_t*)current + HEADER_SIZE);
        }
        current = current->next;
    }

    return NULL; // Out of memory
}

void kfree(void* ptr) {
    if (!ptr) return;

    block_header_t* header = (block_header_t*)((uint8_t*)ptr - HEADER_SIZE);
    header->is_free = true;

    // Coalesce adjacent free blocks
    block_header_t* current = heap_start;
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            current->size += HEADER_SIZE + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void* kcalloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = kmalloc(total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void* krealloc(void* ptr, size_t size) {
    if (!ptr) return kmalloc(size);
    if (size == 0) {
        kfree(ptr);
        return NULL;
    }

    block_header_t* header = (block_header_t*)((uint8_t*)ptr - HEADER_SIZE);
    if (header->size >= size) {
        return ptr;
    }

    void* new_ptr = kmalloc(size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, header->size);
        kfree(ptr);
    }
    return new_ptr;
}

heap_stats_t heap_get_stats(void) {
    heap_stats_t stats = {0, 0, 0, 0};
    stats.total_size = heap_total_size;

    block_header_t* current = heap_start;
    while (current) {
        if (current->is_free) {
            stats.free_size += current->size;
        } else {
            stats.used_size += current->size;
            stats.num_allocations++;
        }
        current = current->next;
    }
    return stats;
}
