#include "../include/net.h"
#include "../include/pci.h"
#include "../include/io.h"
#include "../include/string.h"
#include "../include/stdio.h"
#include "../include/serial.h"
#include "../include/physical.h"
#include "../include/heap.h"

#define E1000_VENDOR_INTEL 0x8086
#define E1000_DEV_82540EM  0x100E
#define E1000_DEV_82545EM  0x100F
#define E1000_DEV_82543GC  0x1004

// Registers
#define REG_CTRL        0x00000
#define REG_STATUS      0x00008
#define REG_EEPROM      0x00014
#define REG_ICR         0x000C0
#define REG_IMS         0x000D0
#define REG_RCTL        0x00100
#define REG_TCTL        0x00400
#define REG_RDBAL       0x02800
#define REG_RDBAH       0x02804
#define REG_RDLEN       0x02808
#define REG_RDH         0x02810
#define REG_RDT         0x02818
#define REG_TDBAL       0x03800
#define REG_TDBAH       0x03804
#define REG_TDLEN       0x03808
#define REG_TDH         0x03810
#define REG_TDT         0x03818
#define REG_MTA         0x05200
#define REG_RAL         0x05400
#define REG_RAH         0x05408

// Flags
#define RCTL_EN         (1 << 1)
#define RCTL_SBP        (1 << 2)
#define RCTL_UPE        (1 << 3)
#define RCTL_MPE        (1 << 4)
#define RCTL_LPE        (1 << 5)
#define RCTL_BAM        (1 << 15)
#define RCTL_BSIZE_2048 (0 << 16)
#define RCTL_SECRC      (1 << 26)

#define TCTL_EN         (1 << 1)
#define TCTL_PSP        (1 << 3)

#define E1000_NUM_RX_DESC 32
#define E1000_NUM_TX_DESC 32
#define E1000_PKT_BUF_SIZE 2048

typedef struct {
    uint64_t address;
    uint16_t length;
    uint16_t checksum;
    uint8_t  status;
    uint8_t  errors;
    uint16_t special;
} __attribute__((packed)) rx_desc_t;

typedef struct {
    uint64_t address;
    uint16_t length;
    uint8_t  cso;
    uint8_t  cmd;
    uint8_t  status;
    uint8_t  css;
    uint16_t special;
} __attribute__((packed)) tx_desc_t;

static bool e1000_present = false;
static uint64_t mmio_base = 0;
static uint8_t mac_addr[6] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56}; // fallback QEMU default

static rx_desc_t* rx_descs = NULL;
static tx_desc_t* tx_descs = NULL;
static uint8_t* rx_buffers[E1000_NUM_RX_DESC];
static uint8_t* tx_buffers[E1000_NUM_TX_DESC];

static uint32_t rx_cur = 0;
static uint32_t tx_cur = 0;

static inline void e1000_write32(uint32_t reg, uint32_t val) {
    if (mmio_base) {
        *((volatile uint32_t*)(mmio_base + reg)) = val;
    }
}

static inline uint32_t e1000_read32(uint32_t reg) {
    if (mmio_base) {
        return *((volatile uint32_t*)(mmio_base + reg));
    }
    return 0;
}

static uint16_t e1000_read_eeprom(uint8_t addr) {
    e1000_write32(REG_EEPROM, 1 | ((uint32_t)addr << 8));
    uint32_t tmp = 0;
    while (!((tmp = e1000_read32(REG_EEPROM)) & (1 << 4))) {
        // Spin until EEPROM read done
    }
    return (uint16_t)((tmp >> 16) & 0xFFFF);
}

static void e1000_read_mac(void) {
    uint16_t val = e1000_read_eeprom(0);
    mac_addr[0] = (uint8_t)(val & 0xFF);
    mac_addr[1] = (uint8_t)(val >> 8);

    val = e1000_read_eeprom(1);
    mac_addr[2] = (uint8_t)(val & 0xFF);
    mac_addr[3] = (uint8_t)(val >> 8);

    val = e1000_read_eeprom(2);
    mac_addr[4] = (uint8_t)(val & 0xFF);
    mac_addr[5] = (uint8_t)(val >> 8);

    // If EEPROM read returned 0 or all 1s, check RAL/RAH registers
    if ((mac_addr[0] == 0 && mac_addr[1] == 0 && mac_addr[2] == 0) ||
        (mac_addr[0] == 0xFF && mac_addr[1] == 0xFF)) {
        uint32_t ral = e1000_read32(REG_RAL);
        uint32_t rah = e1000_read32(REG_RAH);
        if (ral != 0) {
            mac_addr[0] = (uint8_t)(ral & 0xFF);
            mac_addr[1] = (uint8_t)((ral >> 8) & 0xFF);
            mac_addr[2] = (uint8_t)((ral >> 16) & 0xFF);
            mac_addr[3] = (uint8_t)((ral >> 24) & 0xFF);
            mac_addr[4] = (uint8_t)(rah & 0xFF);
            mac_addr[5] = (uint8_t)((rah >> 8) & 0xFF);
        }
    }
}

void e1000_init(void) {
    // Scan PCI for Intel E1000 NIC
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t dev = 0; dev < 32; dev++) {
            for (uint8_t func = 0; func < 8; func++) {
                uint16_t vendor = pci_read_config_word((uint8_t)bus, dev, func, 0x00);
                if (vendor == 0xFFFF) {
                    if (func == 0) break;
                    continue;
                }
                uint16_t device = pci_read_config_word((uint8_t)bus, dev, func, 0x02);

                if (vendor == E1000_VENDOR_INTEL &&
                    (device == E1000_DEV_82540EM || device == E1000_DEV_82545EM || device == E1000_DEV_82543GC)) {
                    
                    uint32_t bar0 = pci_read_config_dword((uint8_t)bus, dev, func, 0x10);
                    mmio_base = (uint64_t)(bar0 & ~0xF);

                    // Enable Bus Master and Memory Space in PCI Command register
                    uint16_t pci_cmd = pci_read_config_word((uint8_t)bus, dev, func, 0x04);
                    pci_cmd |= 0x0006; // Bus Master (0x04) + Memory Space (0x02)
                    // Write back via I/O config
                    uint32_t addr = (uint32_t)((bus << 16) | (dev << 11) | (func << 8) | 0x04 | 0x80000000);
                    outl(PCI_CONFIG_ADDRESS, addr);
                    outw(PCI_CONFIG_DATA, pci_cmd);

                    e1000_present = true;
                    serial_printf("E1000: Found Intel NIC (Device ID 0x%04x) at MMIO 0x%08x\n",
                                  device, (uint32_t)mmio_base);
                    goto found_nic;
                }
            }
        }
    }

found_nic:
    if (!e1000_present) {
        serial_printf("E1000: No supported Intel E1000 network card detected\n");
        return;
    }

    // Read MAC
    e1000_read_mac();
    serial_printf("E1000: MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
                  mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    // Setup Multicast Table Array
    for (int i = 0; i < 128; i++) {
        e1000_write32(REG_MTA + (i * 4), 0);
    }

    // Allocate RX Descriptors & Buffers
    rx_descs = (rx_desc_t*)kmalloc(sizeof(rx_desc_t) * E1000_NUM_RX_DESC + 16);
    memset(rx_descs, 0, sizeof(rx_desc_t) * E1000_NUM_RX_DESC);

    for (int i = 0; i < E1000_NUM_RX_DESC; i++) {
        rx_buffers[i] = (uint8_t*)kmalloc(E1000_PKT_BUF_SIZE);
        rx_descs[i].address = (uint64_t)rx_buffers[i];
        rx_descs[i].status = 0;
    }

    e1000_write32(REG_RDBAL, (uint32_t)((uint64_t)rx_descs & 0xFFFFFFFF));
    e1000_write32(REG_RDBAH, (uint32_t)(((uint64_t)rx_descs >> 32) & 0xFFFFFFFF));
    e1000_write32(REG_RDLEN, E1000_NUM_RX_DESC * sizeof(rx_desc_t));
    e1000_write32(REG_RDH, 0);
    e1000_write32(REG_RDT, E1000_NUM_RX_DESC - 1);
    e1000_write32(REG_RCTL, RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_BAM | RCTL_BSIZE_2048 | RCTL_SECRC);

    // Allocate TX Descriptors & Buffers
    tx_descs = (tx_desc_t*)kmalloc(sizeof(tx_desc_t) * E1000_NUM_TX_DESC + 16);
    memset(tx_descs, 0, sizeof(tx_desc_t) * E1000_NUM_TX_DESC);

    for (int i = 0; i < E1000_NUM_TX_DESC; i++) {
        tx_buffers[i] = (uint8_t*)kmalloc(E1000_PKT_BUF_SIZE);
        tx_descs[i].address = (uint64_t)tx_buffers[i];
        tx_descs[i].status = 1; // DD (Descriptor Done) set initially
        tx_descs[i].cmd = 0;
    }

    e1000_write32(REG_TDBAL, (uint32_t)((uint64_t)tx_descs & 0xFFFFFFFF));
    e1000_write32(REG_TDBAH, (uint32_t)(((uint64_t)tx_descs >> 32) & 0xFFFFFFFF));
    e1000_write32(REG_TDLEN, E1000_NUM_TX_DESC * sizeof(tx_desc_t));
    e1000_write32(REG_TDH, 0);
    e1000_write32(REG_TDT, 0);
    e1000_write32(REG_TCTL, TCTL_EN | TCTL_PSP | (0x0F << 4) | (0x40 << 12));

    // Clear and disable interrupts for polling mode
    e1000_write32(REG_IMS, 0);
    e1000_read32(REG_ICR);

    rx_cur = 0;
    tx_cur = 0;

    serial_printf("E1000: Link operational and initialized successfully\n");
}

bool e1000_is_present(void) {
    return e1000_present;
}

const uint8_t* e1000_get_mac(void) {
    return mac_addr;
}

void e1000_send_packet(const uint8_t* data, uint16_t len) {
    if (!e1000_present || !tx_descs || len > E1000_PKT_BUF_SIZE) return;

    memcpy(tx_buffers[tx_cur], data, len);
    tx_descs[tx_cur].length = len;
    tx_descs[tx_cur].cmd = (1 << 0) | (1 << 3); // EOP (End of Packet) | RS (Report Status)
    tx_descs[tx_cur].status = 0;

    uint32_t old_cur = tx_cur;
    tx_cur = (tx_cur + 1) % E1000_NUM_TX_DESC;
    e1000_write32(REG_TDT, tx_cur);

    // Wait for transmit to complete
    while (!(tx_descs[old_cur].status & 0x01)) {
        // Spin
    }
}

void e1000_poll_rx(void) {
    if (!e1000_present || !rx_descs) return;

    while (rx_descs[rx_cur].status & 0x01) { // DD (Descriptor Done)
        uint16_t len = rx_descs[rx_cur].length;
        uint8_t* buf = rx_buffers[rx_cur];

        if (len > 0) {
            net_handle_rx_packet(buf, len);
        }

        rx_descs[rx_cur].status = 0;
        uint32_t old_cur = rx_cur;
        rx_cur = (rx_cur + 1) % E1000_NUM_RX_DESC;
        e1000_write32(REG_RDT, old_cur);
    }
}
