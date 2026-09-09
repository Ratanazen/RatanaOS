#ifndef PHYSICAL_H
#define PHYSICAL_H

#include "types.h"

#define PAGE_SIZE 4096

#include "multiboot.h"
void phys_init(multiboot_info_t* mbi);
void* phys_alloc_page(void);
void phys_free_page(void* ptr);
size_t phys_get_free_pages(void);
size_t phys_get_total_pages(void);

#endif // PHYSICAL_H
