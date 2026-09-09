#include "../../include/elf.h"
#include "../../include/physical.h"
#include "../../include/virtual.h"
#include "../../include/string.h"
#include "../../include/serial.h"

process_t* elf_load_and_run(vfs_node_t* file, const char* name) {
    (void)name;
    if (!file || !file->data) return NULL;

    elf64_header_t* header = (elf64_header_t*)file->data;
    if (header->magic != ELF_MAGIC) {
        serial_printf("ELF: Invalid magic for %s\n", file->name);
        return NULL;
    }

    if (header->bitness != 2) { // 64-bit
        serial_printf("ELF: Not 64-bit\n");
        return NULL;
    }

    // Create a new process (user mode)
    process_t* proc = process_create((void*)header->entry, true);
    if (!proc) return NULL;

    elf64_program_header_t* ph = (elf64_program_header_t*)(file->data + header->program_header_pos);

    for (int i = 0; i < header->program_header_entries; i++) {
        if (ph[i].type == 1) { // PT_LOAD
            uint64_t vaddr = ph[i].virtual_addr;
            uint64_t mem_size = ph[i].memory_size;
            uint64_t file_size = ph[i].file_size;

            uint64_t page_start = vaddr & ~0xFFF;
            uint64_t page_end = (vaddr + mem_size + 0xFFF) & ~0xFFF;

            // Allocate and map pages
            for (uint64_t addr = page_start; addr < page_end; addr += PAGE_SIZE) {
                uint64_t phys = (uint64_t)phys_alloc_page();
                vmm_map_page(proc->pml4, addr, phys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
            }

            // Temporarily map to current address space to copy data?
            // Since kernel is identity mapped and we are in kernel, 
            // we can just temporarily switch CR3 to proc->pml4, copy data, and switch back.
            // But wait, the file->data is in kernel space! If we switch CR3, can we still read it?
            // Yes, because kernel is identity mapped in ALL PML4s!
            
            uint64_t current_cr3;
            __asm__ volatile("mov %%cr3, %0" : "=r"(current_cr3));
            
            vmm_switch_address_space(proc->pml4);
            
            memset((void*)vaddr, 0, mem_size);
            memcpy((void*)vaddr, file->data + ph[i].offset, file_size);
            
            __asm__ volatile("mov %0, %%cr3" :: "r"(current_cr3));
        }
    }

    serial_printf("ELF: Loaded %s (PID %d, Entry 0x%x)\n", file->name, proc->pid, header->entry);
    return proc;
}

int elf_execve(vfs_node_t* file, trap_frame_t* regs) {
    if (!file || !file->data) return -1;

    elf64_header_t* header = (elf64_header_t*)file->data;
    if (header->magic != ELF_MAGIC || header->bitness != 2) return -1;

    // Create a NEW address space
    pml4_t* new_pml4 = vmm_create_address_space();
    if (!new_pml4) return -1;

    elf64_program_header_t* ph = (elf64_program_header_t*)(file->data + header->program_header_pos);

    for (int i = 0; i < header->program_header_entries; i++) {
        if (ph[i].type == 1) { // PT_LOAD
            uint64_t vaddr = ph[i].virtual_addr;
            uint64_t mem_size = ph[i].memory_size;
            uint64_t file_size = ph[i].file_size;

            uint64_t page_start = vaddr & ~0xFFF;
            uint64_t page_end = (vaddr + mem_size + 0xFFF) & ~0xFFF;

            for (uint64_t addr = page_start; addr < page_end; addr += PAGE_SIZE) {
                uint64_t phys = (uint64_t)phys_alloc_page();
                vmm_map_page(new_pml4, addr, phys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
            }

            uint64_t current_cr3;
            __asm__ volatile("mov %%cr3, %0" : "=r"(current_cr3));
            
            vmm_switch_address_space(new_pml4);
            memset((void*)vaddr, 0, mem_size);
            memcpy((void*)vaddr, file->data + ph[i].offset, file_size);
            __asm__ volatile("mov %0, %%cr3" :: "r"(current_cr3));
        }
    }

    // Set up a new user stack in the new address space
    for (int i = 0; i < 4; i++) {
        uint64_t phys = (uint64_t)phys_alloc_page();
        vmm_map_page(new_pml4, 0x00007FFFFFFFF000ULL - (i * PAGE_SIZE), phys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
    }

    // Replace the current process address space
    current_process->pml4 = new_pml4;
    vmm_switch_address_space(new_pml4); // Activate immediately

    // Modify the trap frame so iretq jumps to the new entry point
    regs->rip = header->entry;
    regs->rsp = 0x00007FFFFFFFF000ULL;
    regs->cs = 0x18 | 3;
    regs->ss = 0x20 | 3;
    regs->rflags = 0x202; // IF = 1

    return 0; // Success
}
