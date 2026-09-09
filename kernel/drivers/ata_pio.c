#include "ata_pio.h"
#include "common_headers/io.h"
#include "utilities/iolayer.h"
#include "utilities/iolayer.h"

#include <stdbool.h>

ata_drive_t ata_drives[ATA_MAX_DRIVES];
ata_drive_t *ata_active_drive_ptr = 0;
uint8_t ata_drive_count = 0;

// Utils

static void ata_pio_settle(uint16_t base)
{
    inb(base + 0x206);
    inb(base + 0x206);
    inb(base + 0x206);
    inb(base + 0x206);
}

void ata_disk_select(uint8_t disk)
{
    ata_drive_t* d = &ata_drives[disk];
    outb(d->base + 6, d->drive_sel);
    ata_pio_settle(d->base);
    ata_active_drive_ptr = d;
}

static int ata_pio_wait_bsy_clear(uint16_t base)
{
    while ((inb(base + 7) & ATA_STATUS_BSY) != 0);
    return 0;
}

static int ata_pio_wait_drq(uint16_t base)
{
    uint8_t status;
    while (((status = inb(base + 7)) & ATA_STATUS_DRQ) == 0)
    {
        if ((status & ATA_STATUS_ERR) != 0)
        {
            return -1;
        }
    }
    return 0;
}

// Driver

void ata_pio_init(void)
{
    print_line("[Kernel] - Initializing ATA PIO");
    ata_drive_count = 0;

    for (uint8_t drive = 0; drive < ATA_MAX_DRIVES; drive++)
    {
        uint8_t bus = drive / 2;
        uint8_t is_slave = drive % 2;

        ata_drives[drive].base = (bus == 0) ? ATA_PRIMARY_DATA : ATA_SECONDARY_DATA;
        ata_drives[drive].drive_sel = is_slave ? ATA_DRIVE_SLAVE : ATA_DRIVE_MASTER;
        ata_drives[drive].present = 0;

        if (ata_pio_identify(drive))
        {
            ata_drive_count++;
        }
    }

    print_line("[Kernel] - Initalized ATA PIO");
}

int ata_pio_read_sector(uint8_t drive, uint32_t lba, uint8_t* buffer)
{
    if (drive >= ATA_MAX_DRIVES)
    {
        return -1;
    }

    ata_drive_t* d = &ata_drives[drive];

    if (!d->present)
    {
        return -1;
    }

    uint8_t select = d->drive_sel | ((lba >> 24) & 0x0F);
    outb(d->base + 6, select);
    ata_pio_settle(d->base);

    outb(d->base + 2, 1);
    outb(d->base + 3, (uint8_t)(lba & 0xFF));
    outb(d->base + 4, (uint8_t)((lba >> 8) & 0xFF));
    outb(d->base + 5, (uint8_t)((lba >> 16) & 0xFF));

    outb(d->base + 7, 0x20);

    if (ata_pio_wait_bsy_clear(d->base) != 0)
    {
        return -1;
    }

    if (ata_pio_wait_drq(d->base) != 0)
    {
        return -1;
    }

    for (int i = 0; i < 256; i++)
    {
        uint16_t word = inw(d->base);
        buffer[i * 2] = (uint8_t)(word & 0xFF);
        buffer[i * 2 + 1] = (uint8_t)((word >> 8) & 0xFF);
    }

    return 0;
}

int ata_pio_write_sector(uint8_t drive, uint32_t lba, const uint8_t* buffer)
{
    if (drive >= ATA_MAX_DRIVES)
    {
        return -1;
    }

    ata_drive_t* d = &ata_drives[drive];

    if (!d->present)
    {
        return -1;
    }

    uint8_t select = d->drive_sel | ((lba >> 24) & 0x0F);
    outb(d->base + 6, select);
    ata_pio_settle(d->base);

    outb(d->base + 2, 1);
    outb(d->base + 3, (uint8_t)(lba & 0xFF));
    outb(d->base + 4, (uint8_t)((lba >> 8) & 0xFF));
    outb(d->base + 5, (uint8_t)((lba >> 16) & 0xFF));

    outb(d->base + 7, 0x30);

    if (ata_pio_wait_bsy_clear(d->base) != 0)
    {
        return -1;
    }

    if (ata_pio_wait_drq(d->base) != 0)
    {
        return -1;
    }

    for (int i = 0; i < 256; i++)
    {
        uint16_t word = (uint16_t)buffer[i * 2] | ((uint16_t)buffer[i * 2 + 1] << 8);
        outw(d->base, word);
    }

    outb(d->base + 7, 0xE7);

    if (ata_pio_wait_bsy_clear(d->base) != 0)
    {
        return -1;
    }

    return 0;
}

bool ata_pio_identify(uint8_t disk)
{
    ata_drive_t* d = &ata_drives[disk];

    ata_disk_select(disk);

    outb(d->base + 2, 0);
    outb(d->base + 3, 0);
    outb(d->base + 4, 0);
    outb(d->base + 5, 0);

    outb(d->base + 7, 0xEC);

    uint8_t status = inb(d->base + 7);
    if (status == 0)
    {
        d->present = 0;
        return false;
    }

    while ((inb(d->base + 7) & ATA_STATUS_BSY) != 0);

    if (inb(d->base + 4) != 0 || inb(d->base + 5) != 0)
    {
        d->present = 0;
        return false;
    }

    while (((status = inb(d->base + 7)) & ATA_STATUS_DRQ) == 0)
    {
        if ((status & ATA_STATUS_ERR) != 0)
        {
            d->present = 0;
            return false;
        }
    }

    uint16_t identify_data[256];
    for (int i = 0; i < 256; i++)
    {
        identify_data[i] = inw(d->base);
    }

    d->lba28_sectors = ((uint32_t)identify_data[61] << 16) | identify_data[60];
    d->present = 1;

    return true;
}