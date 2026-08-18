#include "../include/cpuid.h"
#include "../include/stdio.h"
#include "../include/string.h"

static inline void cpuid(uint32_t code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    __asm__ volatile ("cpuid"
                      : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                      : "a"(code));
}

static cpu_info_t global_cpu_info;

void cpuid_init(void) {
    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;

    memset(&global_cpu_info, 0, sizeof(cpu_info_t));

    // Get Vendor ID
    cpuid(0, &eax, &ebx, &ecx, &edx);
    memcpy(&global_cpu_info.vendor[0], &ebx, 4);
    memcpy(&global_cpu_info.vendor[4], &edx, 4);
    memcpy(&global_cpu_info.vendor[8], &ecx, 4);
    global_cpu_info.vendor[12] = '\0';

    // Get Features & Family/Model
    if (eax >= 1) {
        cpuid(1, &eax, &ebx, &ecx, &edx);
        global_cpu_info.stepping = eax & 0xF;
        global_cpu_info.model    = (eax >> 4) & 0xF;
        global_cpu_info.family   = (eax >> 8) & 0xF;

        global_cpu_info.has_fpu             = (edx & (1 << 0)) != 0;
        global_cpu_info.has_tsc             = (edx & (1 << 4)) != 0;
        global_cpu_info.has_msr             = (edx & (1 << 5)) != 0;
        global_cpu_info.has_apic            = (edx & (1 << 9)) != 0;
        global_cpu_info.has_cmov            = (edx & (1 << 15)) != 0;
        global_cpu_info.has_mmx             = (edx & (1 << 23)) != 0;
        global_cpu_info.has_sse             = (edx & (1 << 25)) != 0;
        global_cpu_info.has_sse2            = (edx & (1 << 26)) != 0;
        global_cpu_info.has_hyperthreading  = (edx & (1 << 28)) != 0;
        global_cpu_info.has_sse3            = (ecx & (1 << 0)) != 0;
    }

    // Extended brand string
    cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
    if (eax >= 0x80000004) {
        uint32_t brand[12];
        cpuid(0x80000002, &brand[0], &brand[1], &brand[2], &brand[3]);
        cpuid(0x80000003, &brand[4], &brand[5], &brand[6], &brand[7]);
        cpuid(0x80000004, &brand[8], &brand[9], &brand[10], &brand[11]);
        memcpy(global_cpu_info.brand, brand, 48);
        global_cpu_info.brand[48] = '\0';
    } else {
        strcpy(global_cpu_info.brand, "x86 Compatible Processor");
    }
}

cpu_info_t cpuid_get_info(void) {
    return global_cpu_info;
}

void cpuid_print_info(void) {
    kprintf("\nProcessor Information (CPUID):\n");
    kprintf(" Vendor:     %s\n", global_cpu_info.vendor);
    kprintf(" Brand:      %s\n", global_cpu_info.brand);
    kprintf(" Family:     %u | Model: %u | Stepping: %u\n",
            global_cpu_info.family, global_cpu_info.model, global_cpu_info.stepping);
    kprintf(" Features:   [");
    if (global_cpu_info.has_fpu)  kprintf(" FPU");
    if (global_cpu_info.has_tsc)  kprintf(" TSC");
    if (global_cpu_info.has_msr)  kprintf(" MSR");
    if (global_cpu_info.has_apic) kprintf(" APIC");
    if (global_cpu_info.has_cmov) kprintf(" CMOV");
    if (global_cpu_info.has_mmx)  kprintf(" MMX");
    if (global_cpu_info.has_sse)  kprintf(" SSE");
    if (global_cpu_info.has_sse2) kprintf(" SSE2");
    if (global_cpu_info.has_sse3) kprintf(" SSE3");
    if (global_cpu_info.has_hyperthreading) kprintf(" HTT");
    kprintf(" ]\n");
}
