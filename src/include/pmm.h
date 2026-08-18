#ifndef PMM_H
#define PMM_H

#include "types.h"

#define PAGE_SIZE 4096

void pmm_init(uint64_t mem_size_bytes);
void* pmm_alloc_page(void);
void pmm_free_page(void* ptr);
size_t pmm_get_free_pages(void);
size_t pmm_get_total_pages(void);

#endif // PMM_H
