#ifndef RATANAOS_ACPI_H
#define RATANAOS_ACPI_H

#include "types.h"
#include <stdbool.h>

// RSDP (Root System Description Pointer) Structure
typedef struct {
    char signature[8];       // "RSD PTR "
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;        // 0 for ACPI 1.0, 2 for ACPI 2.0+
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__((packed)) acpi_rsdp_t;

// Standard ACPI SDT Header
typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed)) acpi_sdt_header_t;

// RSDT Table
typedef struct {
    acpi_sdt_header_t header;
    uint32_t tables[];
} __attribute__((packed)) acpi_rsdt_t;

// XSDT Table
typedef struct {
    acpi_sdt_header_t header;
    uint64_t tables[];
} __attribute__((packed)) acpi_xsdt_t;

// MADT (Multiple APIC Description Table)
typedef struct {
    acpi_sdt_header_t header;
    uint32_t lapic_address;
    uint32_t flags;
    uint8_t entries[];
} __attribute__((packed)) acpi_madt_t;

// MADT Record Header
typedef struct {
    uint8_t type;
    uint8_t length;
} __attribute__((packed)) acpi_madt_record_t;

// MADT Type 0: Processor Local APIC
typedef struct {
    acpi_madt_record_t header;
    uint8_t acpi_processor_id;
    uint8_t apic_id;
    uint32_t flags; // Bit 0: Enabled
} __attribute__((packed)) acpi_madt_lapic_t;

// MADT Type 1: I/O APIC
typedef struct {
    acpi_madt_record_t header;
    uint8_t ioapic_id;
    uint8_t reserved;
    uint32_t ioapic_address;
    uint32_t global_system_interrupt_base;
} __attribute__((packed)) acpi_madt_ioapic_t;

// FADT (Fixed ACPI Description Table)
typedef struct {
    acpi_sdt_header_t header;
    uint32_t firmware_ctrl;
    uint32_t dsdt;
    uint8_t reserved;
    uint8_t preferred_pm_profile;
    uint16_t sci_int;
    uint32_t smi_cmd;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4bios_req;
    uint8_t pstate_cnt;
    uint32_t pm1a_evt_blk;
    uint32_t pm1b_evt_blk;
    uint32_t pm1a_cnt_blk;
    uint32_t pm1b_cnt_blk;
    uint32_t pm2_cnt_blk;
    uint32_t pm_tmr_blk;
    uint32_t gpe0_blk;
    uint32_t gpe1_blk;
    uint8_t pm1_evt_len;
    uint8_t pm1_cnt_len;
    uint8_t pm2_cnt_len;
    uint8_t pm_tmr_len;
    uint8_t gpe0_len;
    uint8_t gpe1_len;
    uint8_t gpe1_base;
    uint8_t cst_cnt;
    uint16_t p_lvl2_lat;
    uint16_t p_lvl3_lat;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alrm;
    uint8_t mon_alrm;
    uint8_t century;
    uint16_t iapc_boot_arch;
    uint8_t reserved2;
    uint32_t flags;
    uint8_t reset_reg[12];
    uint8_t reset_value;
    uint8_t reserved3[3];
    uint64_t x_firmware_ctrl;
    uint64_t x_dsdt;
    uint8_t x_pm1a_evt_blk[12];
    uint8_t x_pm1b_evt_blk[12];
    uint8_t x_pm1a_cnt_blk[12];
    uint8_t x_pm1b_cnt_blk[12];
    uint8_t x_pm2_cnt_blk[12];
    uint8_t x_pm_tmr_blk[12];
    uint8_t x_gpe0_blk[12];
    uint8_t x_gpe1_blk[12];
} __attribute__((packed)) acpi_fadt_t;

// ACPI Management API
void acpi_init(void);
acpi_sdt_header_t* acpi_find_table(const char* signature);
uint32_t acpi_get_lapic_addr(void);
uint32_t acpi_get_ioapic_addr(void);
int acpi_get_cpu_count(void);
uint8_t acpi_get_cpu_apic_id(int index);
void acpi_poweroff(void);
void acpi_reboot(void);

#endif // RATANAOS_ACPI_H
