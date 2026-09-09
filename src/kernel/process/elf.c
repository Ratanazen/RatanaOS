#include "../../include/elf.h"
#include "../../include/physical.h"
#include "../../include/virtual.h"
#include "../../include/string.h"
#include "../../include/serial.h"
#include "../../include/process.h"
#include "../../include/vfs.h"
#include "../../include/stdio.h"

static uint64_t load_interpreter(const char* path, pml4_t* pml4) {
    serial_printf("DEBUG: load_interpreter called with path: %s\n", path);
    char full_path[256]; 
    strcpy(full_path, "/system/debian"); 
    strcat(full_path, path);
    
    vfs_node_t* interp = vfs_open(full_path);
if (interp && interp->data) {
    serial_printf("SANITY: interp->data pointer is 0x%x\n", (uint32_t)(uint64_t)interp->data);
}
if (interp && interp->data) {
    serial_printf("SANITY: ld.so.2 bytes at 0x21860: %x %x %x %x\n", interp->data[0x21860], interp->data[0x21861], interp->data[0x21862], interp->data[0x21863]);
}
    if (!interp) {
        strcpy(full_path, "/mnt/debian");
        strcat(full_path, path);
        interp = vfs_open(full_path);
    }
    
    if (!interp || !interp->data) {
        serial_printf("ELF: Interpreter not found!\n");
        return 0;
    }

    elf64_header_t* header = (elf64_header_t*)interp->data;
    if (header->magic != ELF_MAGIC || header->bitness != 2) return 0;

    elf64_program_header_t* ph = (elf64_program_header_t*)(interp->data + header->program_header_pos);
    
    uint64_t base_addr = 0x7FC000000000ULL; // High address for ld.so
    
    for (int i = 0; i < header->program_header_entries; i++) {
        if (ph[i].type == 1) { // PT_LOAD
            uint64_t vaddr = ph[i].virtual_addr + base_addr;
            uint64_t mem_size = ph[i].memory_size;
            uint64_t file_size = ph[i].file_size;

            uint64_t page_start = vaddr & ~0xFFF;
            uint64_t page_end = (vaddr + mem_size + 0xFFF) & ~0xFFF;

            for (uint64_t addr = page_start; addr < page_end; addr += PAGE_SIZE) {
                uint64_t phys = (uint64_t)phys_alloc_page();
                vmm_map_page(pml4, addr, phys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
            }
            
            uint64_t current_cr3;
            __asm__ volatile("mov %%cr3, %0" : "=r"(current_cr3));
            
            vmm_switch_address_space(pml4);
            memset((void*)vaddr, 0, mem_size);
            serial_printf("ELF: PT_LOAD vaddr 0x%x, offset 0x%x, filesz 0x%x, memsz 0x%x\n", (uint32_t)vaddr, (uint32_t)ph[i].offset, (uint32_t)file_size, (uint32_t)mem_size);

uint8_t* src_ptr = (uint8_t*)(interp->data + ph[i].offset);
if (vaddr == 0x7FC000001000ULL) {
    serial_printf("ELF: Source bytes at offset 0x20860: %x %x %x %x\n", src_ptr[0x20860], src_ptr[0x20860+1], src_ptr[0x20860+2], src_ptr[0x20860+3]);
}
memcpy((void*)vaddr, interp->data + ph[i].offset, file_size);
            __asm__ volatile("mov %0, %%cr3" :: "r"(current_cr3));
        }
    }
    
    serial_printf("ELF: Loaded interpreter %s at 0x%x\n", path, (uint32_t)(base_addr >> 32)); // print high bits safely
    return base_addr + header->entry;
}

process_t* elf_load_and_run(vfs_node_t* file, const char* name) {
    (void)name;
    if (!file || !file->data) return NULL;

    elf64_header_t* header = (elf64_header_t*)file->data;
    if (header->magic != ELF_MAGIC || header->bitness != 2) return NULL;

    process_t* proc = process_create((void*)header->entry, true);
    if (!proc) return NULL;

    elf64_program_header_t* ph = (elf64_program_header_t*)(file->data + header->program_header_pos);

    char interp_path[256] = {0};
    uint64_t phdr_vaddr = 0;
    
    for (int i = 0; i < header->program_header_entries; i++) {
        if (ph[i].type == 3) {
            strncpy(interp_path, (char*)(file->data + ph[i].offset), 255);
            serial_printf("ELF: Dynamic interpreter requested: %s\n", interp_path);
        }
        if (ph[i].type == 6) {
            phdr_vaddr = ph[i].virtual_addr;
        }
        if (ph[i].type == 1) {
            uint64_t vaddr = ph[i].virtual_addr;
            uint64_t mem_size = ph[i].memory_size;
            uint64_t file_size = ph[i].file_size;
            
            uint64_t load_base = 0;
            if (header->type == 3 && vaddr == 0) { // ET_DYN
                load_base = 0x400000;
                vaddr += load_base;
                if (phdr_vaddr == 0) phdr_vaddr = load_base + header->program_header_pos;
            } else if (header->type == 3) {
                load_base = 0x400000;
                vaddr += load_base;
            }

            uint64_t page_start = vaddr & ~0xFFF;
            uint64_t page_end = (vaddr + mem_size + 0xFFF) & ~0xFFF;

            for (uint64_t addr = page_start; addr < page_end; addr += PAGE_SIZE) {
                uint64_t phys = (uint64_t)phys_alloc_page();
                vmm_map_page(proc->pml4, addr, phys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
            }

            uint64_t current_cr3;
            __asm__ volatile("mov %%cr3, %0" : "=r"(current_cr3));
            vmm_switch_address_space(proc->pml4);
            memset((void*)vaddr, 0, mem_size);
            memcpy((void*)vaddr, file->data + ph[i].offset, file_size);
            __asm__ volatile("mov %0, %%cr3" :: "r"(current_cr3));
        }
    }
    
    if (phdr_vaddr == 0 && header->type != 3) {
        phdr_vaddr = header->program_header_pos;
    }

    uint64_t entry_point = header->entry;
    if (header->type == 3) entry_point += 0x400000;

    if (interp_path[0] != 0) {
        uint64_t interp_entry = load_interpreter(interp_path, proc->pml4);
        if (!interp_entry) {
            serial_printf("ELF: Failed to load interpreter %s\n", interp_path);
            return 0;
        }
        entry_point = interp_entry;
    }


    // Map user stack (4 pages)
    for (int i = 0; i < 4; i++) {
        uint64_t phys = (uint64_t)phys_alloc_page();
        vmm_map_page(proc->pml4, 0x7FFFFFFFF000ULL - (i * PAGE_SIZE), phys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
    }

    // Stack is mapped at 0x7FFFFFFFF000 in proc->pml4. We MUST switch to it before accessing it!

    uint64_t current_cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(current_cr3));
    vmm_switch_address_space(proc->pml4);

    uint64_t* stack = (uint64_t*)0x7FFFFFFFF000ULL;
    
    uint64_t auxv[] = {
        3, phdr_vaddr,
        4, header->program_header_entry_size,
        5, header->program_header_entries,
        6, PAGE_SIZE,
        7, 0x400000, 
        9, header->entry + (header->type == 3 ? 0x400000 : 0),
        33, 0,
        0, 0
    };
    if (interp_path[0] != 0) {
        auxv[7] = 0x7FC000000000ULL;
    }
    
    char* str_ptr = (char*)stack - 16;
    strcpy(str_ptr, "hello");
    stack = (uint64_t*)str_ptr;
    
    stack = (uint64_t*)((uint64_t)stack & ~0xF);
    
    stack -= (sizeof(auxv) / 8);
    memcpy(stack, auxv, sizeof(auxv));
    
    stack--; *stack = 0; // envp terminator
    stack--; *stack = 0; // argv terminator
    stack--; *stack = (uint64_t)str_ptr; // argv[0]
    stack--; *stack = 1; // argc
    
    proc->context->rsp = (uint64_t)stack;
    proc->context->rip = entry_point;
    
    __asm__ volatile("mov %0, %%cr3" :: "r"(current_cr3));

    
    __asm__ volatile("mov %0, %%cr3" :: "r"(proc->pml4));
    uint8_t* ptr = (uint8_t*)entry_point;
    serial_printf("ELF: First bytes at entry: %x %x %x %x\n", ptr[0], ptr[1], ptr[2], ptr[3]);
    __asm__ volatile("mov %0, %%cr3" :: "r"(current_cr3));
    serial_printf("ELF: Loaded %s (PID %d, Entry 0x%x, RSP 0x%x)\n", file->name, proc->pid, (uint32_t)entry_point, (uint32_t)proc->context->rsp); //  (PID %d, Entry 0x%x)\n", file->name, proc->pid, (uint32_t)entry_point);
    return proc;
}

int elf_execve(vfs_node_t* file, trap_frame_t* regs) {
    if (!file || !file->data || !current_process || !regs) return -1;

    elf64_header_t* header = (elf64_header_t*)file->data;
    if (header->magic != ELF_MAGIC || header->bitness != 2) return -1;

    pml4_t* new_pml4 = vmm_create_address_space();
    if (!new_pml4) return -1;

    elf64_program_header_t* ph = (elf64_program_header_t*)(file->data + header->program_header_pos);

    char interp_path[256] = {0};
    uint64_t phdr_vaddr = 0;

    for (int i = 0; i < header->program_header_entries; i++) {
        if (ph[i].type == 3) { // PT_INTERP
            strncpy(interp_path, (char*)(file->data + ph[i].offset), 255);
            serial_printf("EXECVE: Dynamic interpreter requested: %s\n", interp_path);
        }
        if (ph[i].type == 6) { // PT_PHDR
            phdr_vaddr = ph[i].virtual_addr;
        }
        if (ph[i].type == 1) { // PT_LOAD
            uint64_t vaddr = ph[i].virtual_addr;
            uint64_t mem_size = ph[i].memory_size;
            uint64_t file_size = ph[i].file_size;

            uint64_t load_base = 0;
            if (header->type == 3 && vaddr == 0) { // ET_DYN
                load_base = 0x400000;
                vaddr += load_base;
                if (phdr_vaddr == 0) phdr_vaddr = load_base + header->program_header_pos;
            } else if (header->type == 3) {
                load_base = 0x400000;
                vaddr += load_base;
            }

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

    if (phdr_vaddr == 0 && header->type != 3) {
        phdr_vaddr = header->program_header_pos;
    }

    uint64_t entry_point = header->entry;
    if (header->type == 3) entry_point += 0x400000;

    if (interp_path[0] != 0) {
        uint64_t interp_entry = load_interpreter(interp_path, new_pml4);
        if (!interp_entry) {
            serial_printf("EXECVE: Failed to load interpreter %s\n", interp_path);
            return -1;
        }
        entry_point = interp_entry;
    }

    // Map user stack (4 pages)
    for (int i = 0; i < 4; i++) {
        uint64_t phys = (uint64_t)phys_alloc_page();
        vmm_map_page(new_pml4, 0x7FFFFFFFF000ULL - (i * PAGE_SIZE), phys, PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER);
    }

    uint64_t current_cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(current_cr3));
    vmm_switch_address_space(new_pml4);

    uint64_t* stack = (uint64_t*)0x7FFFFFFFF000ULL;

    uint64_t auxv[] = {
        3, phdr_vaddr,
        4, header->program_header_entry_size,
        5, header->program_header_entries,
        6, PAGE_SIZE,
        7, 0x400000,
        9, header->entry + (header->type == 3 ? 0x400000 : 0),
        33, 0,
        0, 0
    };
    if (interp_path[0] != 0) {
        auxv[7] = 0x7FC000000000ULL;
    }

    char* str_ptr = (char*)stack - 32;
    strncpy(str_ptr, file->name, 31);
    str_ptr[31] = '\0';
    stack = (uint64_t*)str_ptr;

    stack = (uint64_t*)((uint64_t)stack & ~0xF);

    stack -= (sizeof(auxv) / 8);
    memcpy(stack, auxv, sizeof(auxv));

    stack--; *stack = 0; // envp terminator
    stack--; *stack = 0; // argv terminator
    stack--; *stack = (uint64_t)str_ptr; // argv[0]
    stack--; *stack = 1; // argc

    current_process->pml4 = new_pml4;
    current_process->brk_start = 0x80000000ULL;
    current_process->brk_end = 0x80000000ULL;

    regs->rsp = (uint64_t)stack;
    regs->rip = entry_point;
    regs->rax = 0;

    vmm_switch_address_space(new_pml4);
    serial_printf("EXECVE: Successfully replaced process (PID %d) with %s (Entry 0x%x, RSP 0x%x)\n",
                  (uint32_t)current_process->pid, file->name, (uint32_t)entry_point, (uint32_t)regs->rsp);

    return 0;
}

