#include "../include/ata.h"
#include "../include/io.h"
#include "../include/serial.h"

static void ata_wait_bsy(void) {
    while(inb(ATA_PRIMARY_IO + 7) & 0x80);
}

static void ata_wait_drq(void) {
    while(!(inb(ATA_PRIMARY_IO + 7) & 0x08));
}

void ata_init(void) {
    // Basic detection or reset could go here
    serial_printf("ATA: Driver initialized (Primary IO: 0x1F0)\\n");
}

void ata_read_sectors(uint32_t lba, uint8_t sector_count, uint8_t* target) {
    ata_wait_bsy();
    
    // Select drive and LBA bits 24-27
    outb(ATA_PRIMARY_IO + 6, 0xE0 | ((lba >> 24) & 0x0F));
    
    outb(ATA_PRIMARY_IO + 2, sector_count); // Sector count
    outb(ATA_PRIMARY_IO + 3, (uint8_t) lba); // LBA low
    outb(ATA_PRIMARY_IO + 4, (uint8_t)(lba >> 8)); // LBA mid
    outb(ATA_PRIMARY_IO + 5, (uint8_t)(lba >> 16)); // LBA high
    
    outb(ATA_PRIMARY_IO + 7, 0x20); // Send read command
    
    for (int j = 0; j < sector_count; j++) {
        ata_wait_bsy();
        ata_wait_drq();
        
        for (int i = 0; i < 256; i++) {
            uint16_t word = inw(ATA_PRIMARY_IO + 0);
            target[i * 2] = (uint8_t)(word & 0xFF);
            target[i * 2 + 1] = (uint8_t)((word >> 8) & 0xFF);
        }
        target += 512;
    }
}

void ata_write_sectors(uint32_t lba, uint8_t sector_count, uint8_t* source) {
    ata_wait_bsy();
    
    outb(ATA_PRIMARY_IO + 6, 0xE0 | ((lba >> 24) & 0x0F));
    
    outb(ATA_PRIMARY_IO + 2, sector_count);
    outb(ATA_PRIMARY_IO + 3, (uint8_t) lba);
    outb(ATA_PRIMARY_IO + 4, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_IO + 5, (uint8_t)(lba >> 16));
    
    outb(ATA_PRIMARY_IO + 7, 0x30); // Send write command
    
    for (int j = 0; j < sector_count; j++) {
        ata_wait_bsy();
        ata_wait_drq();
        
        for (int i = 0; i < 256; i++) {
            uint16_t word = source[i * 2] | (source[i * 2 + 1] << 8);
            outw(ATA_PRIMARY_IO + 0, word);
        }
        source += 512;
    }
}
