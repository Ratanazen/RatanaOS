#include "../include/acpi.h"
#include "../include/string.h"
#include "../include/stdio.h"
#include "../include/serial.h"
#include "../include/io.h"

#define MAX_ACPI_TABLES 32
#define MAX_CPUS 32

static acpi_rsdp_t* rsdp = NULL;
static acpi_rsdt_t* rsdt = NULL;
static acpi_xsdt_t* xsdt = NULL;
static acpi_madt_t* madt = NULL;
static acpi_fadt_t* fadt = NULL;

static acpi_sdt_header_t* sdt_tables[MAX_ACPI_TABLES];
static int num_sdt_tables = 0;

static uint32_t lapic_base_addr = 0xFEE00000;
static uint32_t ioapic_base_addr = 0xFEC00000;
static uint8_t cpu_apic_ids[MAX_CPUS];
static int cpu_count = 0;

static bool acpi_validate_checksum(const void* ptr, int len) {
    const uint8_t* b = (const uint8_t*)ptr;
    uint8_t sum = 0;
    for (int i = 0; i < len; i++) {
        sum += b[i];
    }
    return (sum == 0);
}

static acpi_rsdp_t* acpi_scan_rsdp_range(uint64_t start, uint64_t end) {
    for (uint64_t p = start; p < end; p += 16) {
        if (memcmp((const void*)p, "RSD PTR ", 8) == 0) {
            acpi_rsdp_t* candidate = (acpi_rsdp_t*)p;
            if (acpi_validate_checksum(candidate, sizeof(acpi_rsdp_t))) {
                return candidate;
            }
        }
    }
    return NULL;
}

static void acpi_parse_madt(acpi_madt_t* m) {
    if (!m) return;
    lapic_base_addr = m->lapic_address;
    cpu_count = 0;

    uint8_t* ptr = m->entries;
    uint8_t* end = (uint8_t*)m + m->header.length;

    while (ptr < end) {
        acpi_madt_record_t* rec = (acpi_madt_record_t*)ptr;
        if (rec->length == 0) break; // prevent infinite loop on corrupted entry

        if (rec->type == 0) { // Processor Local APIC
            acpi_madt_lapic_t* lapic = (acpi_madt_lapic_t*)rec;
            if (lapic->flags & 1) { // Enabled
                if (cpu_count < MAX_CPUS) {
                    cpu_apic_ids[cpu_count++] = lapic->apic_id;
                }
            }
        } else if (rec->type == 1) { // I/O APIC
            acpi_madt_ioapic_t* ioapic = (acpi_madt_ioapic_t*)rec;
            ioapic_base_addr = ioapic->ioapic_address;
        }

        ptr += rec->length;
    }

    serial_printf("ACPI: MADT parsed -> LAPIC @ 0x%08x, IOAPIC @ 0x%08x, %d CPU cores detected\n",
                  lapic_base_addr, ioapic_base_addr, cpu_count);
}

void acpi_init(void) {
    // 1. Scan EBDA (Extended BIOS Data Area)
    volatile uint16_t* ebda_ptr = (volatile uint16_t*)(uintptr_t)0x40E;
    uint16_t ebda_seg = *ebda_ptr;
    uint64_t ebda_addr = ((uint64_t)ebda_seg) << 4;
    if (ebda_addr >= 0x80000 && ebda_addr < 0xA0000) {
        rsdp = acpi_scan_rsdp_range(ebda_addr, ebda_addr + 1024);
    }

    // 2. Scan BIOS Read-Only Memory Space (0x000E0000 - 0x000FFFFF)
    if (!rsdp) {
        rsdp = acpi_scan_rsdp_range(0xE0000, 0x100000);
    }

    if (!rsdp) {
        serial_printf("ACPI: Warning - RSDP pointer not found in BIOS memory\n");
        return;
    }

    serial_printf("ACPI: RSDP found at 0x%08x, OEM: %.6s, Revision: %d\n",
                  (uint32_t)(uint64_t)rsdp, rsdp->oem_id, rsdp->revision);

    // 3. Parse RSDT or XSDT
    num_sdt_tables = 0;
    if (rsdp->revision >= 2 && rsdp->xsdt_address != 0) {
        xsdt = (acpi_xsdt_t*)rsdp->xsdt_address;
        if (acpi_validate_checksum(xsdt, xsdt->header.length)) {
            int count = (xsdt->header.length - sizeof(acpi_sdt_header_t)) / 8;
            for (int i = 0; i < count && num_sdt_tables < MAX_ACPI_TABLES; i++) {
                acpi_sdt_header_t* header = (acpi_sdt_header_t*)xsdt->tables[i];
                if (header && acpi_validate_checksum(header, header->length)) {
                    sdt_tables[num_sdt_tables++] = header;
                }
            }
        }
    }

    if (num_sdt_tables == 0 && rsdp->rsdt_address != 0) {
        rsdt = (acpi_rsdt_t*)(uint64_t)rsdp->rsdt_address;
        if (acpi_validate_checksum(rsdt, rsdt->header.length)) {
            int count = (rsdt->header.length - sizeof(acpi_sdt_header_t)) / 4;
            for (int i = 0; i < count && num_sdt_tables < MAX_ACPI_TABLES; i++) {
                acpi_sdt_header_t* header = (acpi_sdt_header_t*)(uint64_t)rsdt->tables[i];
                if (header && acpi_validate_checksum(header, header->length)) {
                    sdt_tables[num_sdt_tables++] = header;
                }
            }
        }
    }

    serial_printf("ACPI: Discovered %d valid SDT tables:\n", num_sdt_tables);
    for (int i = 0; i < num_sdt_tables; i++) {
        char sig[5] = {0};
        memcpy(sig, sdt_tables[i]->signature, 4);
        serial_printf("  [%d] Table '%s' (len %d bytes, OEM: %.6s)\n",
                      i, sig, sdt_tables[i]->length, sdt_tables[i]->oem_id);

        if (memcmp(sig, "APIC", 4) == 0) {
            madt = (acpi_madt_t*)sdt_tables[i];
            acpi_parse_madt(madt);
        } else if (memcmp(sig, "FACP", 4) == 0) {
            fadt = (acpi_fadt_t*)sdt_tables[i];
        }
    }
}

acpi_sdt_header_t* acpi_find_table(const char* signature) {
    if (!signature) return NULL;
    for (int i = 0; i < num_sdt_tables; i++) {
        if (memcmp(sdt_tables[i]->signature, signature, 4) == 0) {
            return sdt_tables[i];
        }
    }
    return NULL;
}

uint32_t acpi_get_lapic_addr(void) {
    return lapic_base_addr;
}

uint32_t acpi_get_ioapic_addr(void) {
    return ioapic_base_addr;
}

int acpi_get_cpu_count(void) {
    return (cpu_count > 0) ? cpu_count : 1;
}

uint8_t acpi_get_cpu_apic_id(int index) {
    if (index >= 0 && index < cpu_count) {
        return cpu_apic_ids[index];
    }
    return 0;
}

void acpi_poweroff(void) {
    serial_printf("ACPI: Initiating system power off...\n");
    if (fadt && fadt->pm1a_cnt_blk) {
        // QEMU ACPI poweroff port 0x604 or FADT PM1a
        outw((uint16_t)fadt->pm1a_cnt_blk, 0x2000 | (0x5 << 10)); // SLP_TYP = 5, SLP_EN = 1
    }
    // Fallback for QEMU / Bochs poweroff
    outw(0x604, 0x2000);
    outw(0xB004, 0x2000);
    outw(0x4004, 0x3400);

    // If still running, halt
    __asm__ volatile("cli; hlt");
}

void acpi_reboot(void) {
    serial_printf("ACPI: Initiating system reboot...\n");
    if (fadt && (fadt->flags & (1 << 10)) && fadt->reset_reg[0] == 1) { // Generic I/O
        uint16_t port = *(uint16_t*)&fadt->reset_reg[4];
        outb(port, fadt->reset_value);
    }
    // 8042 Keyboard Controller pulse reset line
    uint8_t good = 0x02;
    while (good & 0x02) {
        good = inb(0x64);
    }
    outb(0x64, 0xFE);

    // Fallback triple fault
    struct { uint16_t limit; uint64_t base; } __attribute__((packed)) null_idt = {0, 0};
    __asm__ volatile("lidt %0; int3" :: "m"(null_idt));
}
