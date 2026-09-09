#ifndef ATA_PIO_H
#define ATA_PIO_H

#include <stdbool.h>
#include <stdint.h>

/*
0x1F0 - Data Register (16-bit, R/W)
0x1F1 - Error/Features
0x1F2 - Sector Count
0x1F3 - LBA low (bits 0-7)
0x1F4 - LBA mid (bits 8-15)
0x1F5 - LBA high (bits 16-23)
0x1F6 - Drive/Head (bits 24-27 + drive select)
0x1F7 - Command/Status
*/

// Primary bus
#define ATA_PRIMARY_DATA        0x1F0
#define ATA_PRIMARY_ERROR       0x1F1
#define ATA_PRIMARY_SEC_COUNT   0x1F2
#define ATA_PRIMARY_LBA_LOW     0x1F3
#define ATA_PRIMARY_LBA_MID     0x1F4
#define ATA_PRIMARY_LBA_HIGH    0x1F5
#define ATA_PRIMARY_DRIVE_SEL   0x1F6
#define ATA_PRIMARY_COMMAND     0x1F7
#define ATA_PRIMARY_STATUS      0x1F7
#define ATA_PRIMARY_ALT_STATUS  0x3F6

// Secondary bus
#define ATA_SECONDARY_DATA 0x170
#define ATA_SECONDARY_ERROR 0x171
#define ATA_SECONDARY_SEC_COUNT 0x172
#define ATA_SECONDARY_LBA_LOW 0x173
#define ATA_SECONDARY_LBA_MID 0x174
#define ATA_SECONDARY_LBA_HIGH 0x175
#define ATA_SECONDARY_DRIVE_SEL 0x176
#define ATA_SECONDARY_COMMAND 0x177
#define ATA_SECONDARY_STATUS 0x177
#define ATA_SECONDARY_ALT_STATUS 0x376

#define ATA_STATUS_BSY  0x80
#define ATA_STATUS_DRQ  0x08
#define ATA_STATUS_ERR  0x01

#define ATA_DRIVE_MASTER 0xE0
#define ATA_DRIVE_SLAVE  0xF0

#define ATA_MAX_DRIVES 4

typedef struct
{
    uint16_t    base;               // I/O base (0x1F0 o 0x170)
    uint8_t     drive_sel;          // 0xE0 = master, 0xF0 = slave
    uint8_t     present;
    char        name[32];
    uint32_t    lba28_sectors;      // capacity
} ata_drive_t;

extern ata_drive_t ata_drives[ATA_MAX_DRIVES];
extern ata_drive_t *ata_active_drive_ptr;
extern uint8_t ata_drive_count;

void ata_pio_init();
bool ata_pio_identify(uint8_t drive);
int ata_pio_read_sector(uint8_t drive, uint32_t lba, uint8_t* buffer);
int ata_pio_write_sector(uint8_t drive, uint32_t lba, const uint8_t* buffer);

#endif