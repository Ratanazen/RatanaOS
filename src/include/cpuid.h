#ifndef CPUID_H
#define CPUID_H

#include "types.h"

typedef struct {
    char vendor[13];
    char brand[49];
    uint32_t family;
    uint32_t model;
    uint32_t stepping;
    bool has_fpu;
    bool has_tsc;
    bool has_msr;
    bool has_apic;
    bool has_cmov;
    bool has_mmx;
    bool has_sse;
    bool has_sse2;
    bool has_sse3;
    bool has_hyperthreading;
} cpu_info_t;

void cpuid_init(void);
cpu_info_t cpuid_get_info(void);
void cpuid_print_info(void);

#endif // CPUID_H
