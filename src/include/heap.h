#ifndef HEAP_H
#define HEAP_H

#include "types.h"

void heap_init(uint32_t start_addr, size_t size);
void* kmalloc(size_t size);
void* kcalloc(size_t num, size_t size);
void* krealloc(void* ptr, size_t size);
void kfree(void* ptr);

typedef struct {
    size_t total_size;
    size_t used_size;
    size_t free_size;
    size_t num_allocations;
} heap_stats_t;

heap_stats_t heap_get_stats(void);

#endif // HEAP_H
