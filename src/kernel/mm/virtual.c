#include "../../include/virtual.h"
#include "../../include/physical.h"
#include "../../include/string.h"
#include "../../include/serial.h"

// The bootloader sets up a PML4 at a known location, but we will create our own clean kernel PML4.
static pml4_t* kernel_pml4 = NULL;

static inline void invlpg(uint64_t addr) {
    __asm__ volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

void vmm_init(void) {
    kernel_pml4 = (pml4_t*)phys_alloc_page();
    memset(kernel_pml4, 0, PAGE_SIZE);

    // Identity map the first 4GB of RAM for the kernel (using 2MB huge pages)
    // 4GB = 2048 * 2MB pages
    // We need 1 PML4 entry -> 1 PDPT entry -> 4 PD entries -> 2048 PT entries.
    
    uint64_t* pdpt = (uint64_t*)phys_alloc_page();
    memset(pdpt, 0, PAGE_SIZE);
    kernel_pml4[0] = (uint64_t)pdpt | PAGE_PRESENT | PAGE_WRITABLE;

    for (int i = 0; i < 4; i++) {
        uint64_t* pd = (uint64_t*)phys_alloc_page();
        memset(pd, 0, PAGE_SIZE);
        pdpt[i] = (uint64_t)pd | PAGE_PRESENT | PAGE_WRITABLE;

        for (int j = 0; j < 512; j++) {
            uint64_t phys_addr = ((uint64_t)i * 512ULL + (uint64_t)j) * 0x200000ULL; // 2MB blocks
            pd[j] = phys_addr | PAGE_PRESENT | PAGE_WRITABLE | (1 << 7); // Bit 7 = Huge Page
        }
    }

    vmm_switch_address_space(kernel_pml4);
    serial_printf("VMM: Virtual Memory Manager initialized (4GB Identity Mapped)\n");
}

void vmm_switch_address_space(pml4_t* pml4) {
    __asm__ volatile("mov %0, %%cr3" :: "r"((uint64_t)pml4) : "memory");
}

pml4_t* vmm_create_address_space(void) {
    uint64_t* pml4 = (uint64_t*)phys_alloc_page();
    memset(pml4, 0, PAGE_SIZE);
    
    // Allocate a new PDPT for PML4[0]
    uint64_t* pdpt = (uint64_t*)phys_alloc_page();
    memset(pdpt, 0, PAGE_SIZE);
    pml4[0] = (uint64_t)pdpt | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    
    uint64_t* k_pdpt = (uint64_t*)(kernel_pml4[0] & ~0xFFF);
    
    // For pdpt[0] (0 to 1GB), we allocate a dedicated Page Directory
    // so user programs can map things at 0x400000 without sharing the kernel's PD.
    uint64_t* pd0 = (uint64_t*)phys_alloc_page();
    memset(pd0, 0, PAGE_SIZE);
    pdpt[0] = (uint64_t)pd0 | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    
    uint64_t* k_pd0 = (uint64_t*)(k_pdpt[0] & ~0xFFF);
    // Copy the kernel's 2MB mappings into this new PD, preserving their flags (no PAGE_USER).
    if (k_pdpt[0] & PAGE_PRESENT) {
        for (int i = 0; i < 512; i++) {
            pd0[i] = k_pd0[i]; 
        }
    }
    
    // For pdpt[1..3] (1GB to 4GB), we can just share the kernel's PDs.
    for (int i = 1; i < 4; i++) {
        pdpt[i] = k_pdpt[i]; // Share the PDTs for the kernel
    }
    
    return (pml4_t*)pml4;
}

bool vmm_map_page(pml4_t* pml4, uint64_t virtual_addr, uint64_t physical_addr, uint32_t flags) {
    uint16_t pml4_idx = (virtual_addr >> 39) & 0x1FF;
    uint16_t pdpt_idx = (virtual_addr >> 30) & 0x1FF;
    uint16_t pd_idx   = (virtual_addr >> 21) & 0x1FF;
    uint16_t pt_idx   = (virtual_addr >> 12) & 0x1FF;

    if (!(pml4[pml4_idx] & PAGE_PRESENT)) {
        uint64_t pdpt = (uint64_t)phys_alloc_page();
        if (!pdpt) return false;
        memset((void*)pdpt, 0, PAGE_SIZE);
        pml4[pml4_idx] = pdpt | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }
    
    // Ensure PAGE_USER is set on PML4
    if (flags & PAGE_USER) pml4[pml4_idx] |= PAGE_USER;

    uint64_t* pdpt = (uint64_t*)(pml4[pml4_idx] & ~0xFFF);
    if (!(pdpt[pdpt_idx] & PAGE_PRESENT)) {
        uint64_t pd = (uint64_t)phys_alloc_page();
        if (!pd) return false;
        memset((void*)pd, 0, PAGE_SIZE);
        pdpt[pdpt_idx] = pd | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }
    
    // Ensure PAGE_USER is set on PDPT
    if (flags & PAGE_USER) pdpt[pdpt_idx] |= PAGE_USER;

    uint64_t* pd = (uint64_t*)(pdpt[pdpt_idx] & ~0xFFF);
    
    // If the PD entry exists but is a 2MB huge page (kernel mapping), we must shatter it into a 4KB PT!
    if ((pd[pd_idx] & PAGE_PRESENT) && (pd[pd_idx] & 0x80)) { // 0x80 is PAGE_SIZE
        uint64_t pt = (uint64_t)phys_alloc_page();
        if (!pt) return false;
        memset((void*)pt, 0, PAGE_SIZE);
        uint64_t base_phys = pd[pd_idx] & ~0x1FFFFF;
        uint64_t base_flags = pd[pd_idx] & 0xFFF;
        base_flags &= ~0x80; // Remove PAGE_SIZE bit
        
        uint64_t* pt_ptr = (uint64_t*)pt;
        for (int i = 0; i < 512; i++) {
            pt_ptr[i] = (base_phys + i * PAGE_SIZE) | base_flags;
        }
        pd[pd_idx] = pt | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }
    else if (!(pd[pd_idx] & PAGE_PRESENT)) {
        uint64_t pt = (uint64_t)phys_alloc_page();
        if (!pt) return false;
        memset((void*)pt, 0, PAGE_SIZE);
        pd[pd_idx] = pt | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    // Ensure PAGE_USER is set on PD
    if (flags & PAGE_USER) pd[pd_idx] |= PAGE_USER;

    uint64_t* pt = (uint64_t*)(pd[pd_idx] & ~0xFFF);
    pt[pt_idx] = (physical_addr & ~0xFFF) | flags;
    
    return true;
}

void vmm_unmap_page(pml4_t* pml4, uint64_t virtual_addr) {
    uint16_t pml4_idx = (virtual_addr >> 39) & 0x1FF;
    uint16_t pdpt_idx = (virtual_addr >> 30) & 0x1FF;
    uint16_t pd_idx   = (virtual_addr >> 21) & 0x1FF;
    uint16_t pt_idx   = (virtual_addr >> 12) & 0x1FF;

    if (!(pml4[pml4_idx] & PAGE_PRESENT)) return;
    uint64_t* pdpt = (uint64_t*)(pml4[pml4_idx] & ~0xFFF);
    
    if (!(pdpt[pdpt_idx] & PAGE_PRESENT)) return;
    uint64_t* pd = (uint64_t*)(pdpt[pdpt_idx] & ~0xFFF);
    
    if (!(pd[pd_idx] & PAGE_PRESENT)) return;
    uint64_t* pt = (uint64_t*)(pd[pd_idx] & ~0xFFF);
    
    pt[pt_idx] = 0;
    invlpg(virtual_addr);
}

uint64_t vmm_get_physical_address(pml4_t* pml4, uint64_t virtual_addr) {
    uint16_t pml4_idx = (virtual_addr >> 39) & 0x1FF;
    uint16_t pdpt_idx = (virtual_addr >> 30) & 0x1FF;
    uint16_t pd_idx   = (virtual_addr >> 21) & 0x1FF;
    uint16_t pt_idx   = (virtual_addr >> 12) & 0x1FF;

    if (!(pml4[pml4_idx] & PAGE_PRESENT)) return 0;
    uint64_t* pdpt = (uint64_t*)(pml4[pml4_idx] & ~0xFFF);
    
    if (!(pdpt[pdpt_idx] & PAGE_PRESENT)) return 0;
    uint64_t* pd = (uint64_t*)(pdpt[pdpt_idx] & ~0xFFF);
    
    if (!(pd[pd_idx] & PAGE_PRESENT)) return 0;
    if (pd[pd_idx] & (1 << 7)) { // Huge page
        uint64_t phys_base = pd[pd_idx] & ~0x1FFFFF;
        return phys_base + (virtual_addr & 0x1FFFFF);
    }

    uint64_t* pt = (uint64_t*)(pd[pd_idx] & ~0xFFF);
    if (!(pt[pt_idx] & PAGE_PRESENT)) return 0;
    
    return (pt[pt_idx] & ~0xFFF) + (virtual_addr & 0xFFF);
}

pml4_t* vmm_clone_address_space(pml4_t* parent_pml4) {
    pml4_t* child_pml4 = (pml4_t*)phys_alloc_page();
    memset(child_pml4, 0, PAGE_SIZE);

    for (int i = 0; i < 512; i++) {
        if (!(parent_pml4[i] & PAGE_PRESENT)) continue;

        // If it's the kernel region (PML4 entry 0 contains identity map in PDPT 0..3)
        // Actually, we can check PAGE_USER at the PML4 level. 
        // Wait, earlier I patched vmm_create_address_space to add PAGE_USER to pml4[0].
        // So pml4[0] HAS PAGE_USER. We must traverse down to the PDPT level.
        
        uint64_t* parent_pdpt = (uint64_t*)(parent_pml4[i] & ~0xFFF);
        uint64_t* child_pdpt = (uint64_t*)phys_alloc_page();
        memset(child_pdpt, 0, PAGE_SIZE);
        child_pml4[i] = (uint64_t)child_pdpt | (parent_pml4[i] & 0xFFF);

        for (int j = 0; j < 512; j++) {
            if (!(parent_pdpt[j] & PAGE_PRESENT)) continue;

            if (!(parent_pdpt[j] & PAGE_USER)) {
                // Kernel mapping (shallow copy)
                child_pdpt[j] = parent_pdpt[j];
                continue;
            }

            uint64_t* parent_pd = (uint64_t*)(parent_pdpt[j] & ~0xFFF);
            uint64_t* child_pd = (uint64_t*)phys_alloc_page();
            memset(child_pd, 0, PAGE_SIZE);
            child_pdpt[j] = (uint64_t)child_pd | (parent_pdpt[j] & 0xFFF);

            for (int k = 0; k < 512; k++) {
                if (!(parent_pd[k] & PAGE_PRESENT)) continue;

                if (parent_pd[k] & (1 << 7)) { // Huge page
                    child_pd[k] = parent_pd[k];
                    continue;
                }

                uint64_t* parent_pt = (uint64_t*)(parent_pd[k] & ~0xFFF);
                uint64_t* child_pt = (uint64_t*)phys_alloc_page();
                memset(child_pt, 0, PAGE_SIZE);
                child_pd[k] = (uint64_t)child_pt | (parent_pd[k] & 0xFFF);

                for (int l = 0; l < 512; l++) {
                    if (!(parent_pt[l] & PAGE_PRESENT)) continue;

                    uint64_t phys_page = (uint64_t)phys_alloc_page();
                    void* src = (void*)(parent_pt[l] & ~0xFFF);
                    void* dst = (void*)phys_page;
                    memcpy(dst, src, PAGE_SIZE);

                    child_pt[l] = phys_page | (parent_pt[l] & 0xFFF);
                }
            }
        }
    }
    return child_pml4;
}
