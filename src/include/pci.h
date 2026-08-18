#ifndef PCI_H
#define PCI_H

#include "types.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

typedef struct {
    uint8_t  bus;
    uint8_t  device;
    uint8_t  function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t  class_code;
    uint8_t  subclass;
    uint8_t  prog_if;
} pci_device_t;

void pci_init(void);
void pci_scan_all_buses(void);
uint32_t pci_read_config_dword(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
uint16_t pci_read_config_word(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
const char* pci_get_class_name(uint8_t class_code, uint8_t subclass);

#endif // PCI_H
