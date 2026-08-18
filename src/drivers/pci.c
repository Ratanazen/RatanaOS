#include "../include/pci.h"
#include "../include/io.h"
#include "../include/stdio.h"

uint32_t pci_read_config_dword(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    uint32_t address = (uint32_t)((bus << 16) | (dev << 11) | (func << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outb(PCI_CONFIG_ADDRESS, (uint8_t)(address & 0xFF));
    outb(PCI_CONFIG_ADDRESS + 1, (uint8_t)((address >> 8) & 0xFF));
    outb(PCI_CONFIG_ADDRESS + 2, (uint8_t)((address >> 16) & 0xFF));
    outb(PCI_CONFIG_ADDRESS + 3, (uint8_t)((address >> 24) & 0xFF));

    uint32_t tmp = 0;
    tmp |= (uint32_t)inb(PCI_CONFIG_DATA);
    tmp |= ((uint32_t)inb(PCI_CONFIG_DATA + 1)) << 8;
    tmp |= ((uint32_t)inb(PCI_CONFIG_DATA + 2)) << 16;
    tmp |= ((uint32_t)inb(PCI_CONFIG_DATA + 3)) << 24;
    return tmp;
}

uint16_t pci_read_config_word(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    uint32_t address = (uint32_t)((bus << 16) | (dev << 11) | (func << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outb(PCI_CONFIG_ADDRESS, (uint8_t)(address & 0xFF));
    outb(PCI_CONFIG_ADDRESS + 1, (uint8_t)((address >> 8) & 0xFF));
    outb(PCI_CONFIG_ADDRESS + 2, (uint8_t)((address >> 16) & 0xFF));
    outb(PCI_CONFIG_ADDRESS + 3, (uint8_t)((address >> 24) & 0xFF));

    uint16_t tmp = (uint16_t)((inb(PCI_CONFIG_DATA + (offset & 2)) | (inb(PCI_CONFIG_DATA + (offset & 2) + 1) << 8)));
    return tmp;
}

const char* pci_get_class_name(uint8_t class_code, uint8_t subclass) {
    switch (class_code) {
        case 0x00: return "Legacy Device";
        case 0x01:
            if (subclass == 0x01) return "IDE Controller";
            if (subclass == 0x06) return "SATA Controller";
            return "Mass Storage Controller";
        case 0x02:
            if (subclass == 0x00) return "Ethernet Controller";
            return "Network Controller";
        case 0x03:
            if (subclass == 0x00) return "VGA Compatible Adapter";
            return "Display Controller";
        case 0x04: return "Multimedia Controller";
        case 0x05: return "Memory Controller";
        case 0x06:
            if (subclass == 0x00) return "Host Bridge";
            if (subclass == 0x01) return "ISA Bridge";
            if (subclass == 0x04) return "PCI-to-PCI Bridge";
            return "Bridge Device";
        case 0x07: return "Communication Controller";
        case 0x08: return "Generic System Peripheral";
        case 0x09: return "Input Device";
        case 0x0C:
            if (subclass == 0x03) return "USB Controller";
            return "Serial Bus Controller";
        default: return "Other / Unknown Device";
    }
}

void pci_init(void) {
    // PCI bus query ready
}

void pci_scan_all_buses(void) {
    kprintf("\nPCI Bus Devices:\n");
    kprintf(" BUS:DEV:FN  VENDOR:DEV   CLASS  DESCRIPTION\n");
    kprintf(" --------------------------------------------------------\n");

    int found = 0;
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t dev = 0; dev < 32; dev++) {
            for (uint8_t func = 0; func < 8; func++) {
                uint16_t vendor_id = pci_read_config_word((uint8_t)bus, dev, func, 0x00);
                if (vendor_id == 0xFFFF) {
                    if (func == 0) break; // Device not present, skip functions
                    continue;
                }

                uint16_t device_id = pci_read_config_word((uint8_t)bus, dev, func, 0x02);
                uint16_t class_reg = pci_read_config_word((uint8_t)bus, dev, func, 0x0A);
                uint8_t class_code = (uint8_t)((class_reg >> 8) & 0xFF);
                uint8_t subclass   = (uint8_t)(class_reg & 0xFF);

                kprintf(" %02x :%02x :%02x  %04x  :%04x   %02x:%02x  %s\n",
                        bus, dev, func, vendor_id, device_id, class_code, subclass,
                        pci_get_class_name(class_code, subclass));
                found++;
            }
        }
    }
    kprintf(" Total PCI Devices Found: %d\n", found);
}
