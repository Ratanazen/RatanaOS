#ifndef ATA_H
#define ATA_H

#include "types.h"

#define ATA_PRIMARY_IO      0x1F0
#define ATA_PRIMARY_CTRL    0x3F6

void ata_init(void);
void ata_read_sectors(uint32_t lba, uint8_t sector_count, uint8_t* target);
void ata_write_sectors(uint32_t lba, uint8_t sector_count, uint8_t* source);

#endif
