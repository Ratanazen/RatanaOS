#ifndef VIRTUAL_H
#define VIRTUAL_H

#include "types.h"

#define PAGE_PRESENT  (1 << 0)
#define PAGE_WRITABLE (1 << 1)
#define PAGE_USER     (1 << 2)

typedef uint64_t pml4_t;

void vmm_init(void);
pml4_t* vmm_create_address_space(void);
pml4_t* vmm_clone_address_space(pml4_t* parent_pml4);
void vmm_switch_address_space(pml4_t* pml4);
bool vmm_map_page(pml4_t* pml4, uint64_t virtual_addr, uint64_t physical_addr, uint32_t flags);
void vmm_unmap_page(pml4_t* pml4, uint64_t virtual_addr);
uint64_t vmm_get_physical_address(pml4_t* pml4, uint64_t virtual_addr);

#endif
