#include "gdt.h"

#define GDT_ENTRIES 5

static struct gdt_descriptor_32 gdt[GDT_ENTRIES];
static struct gdt_ptr_32 gdt_pointer;

extern void gdt_flush(uint32_t gdt_ptr_addr);

void gdt_init(void) 
{
    gdt_pointer.limit = (sizeof(struct gdt_descriptor_32) * GDT_ENTRIES) - 1;
    gdt_pointer.base  = (uint32_t)&gdt;

    encode_gdt_entry_32(&gdt[0], 0, 0, 0, 0);
    
    // Entry 1: Kernel Code (0x08)
    encode_gdt_entry_32(&gdt[1], 0, 0xFFFFF, 0x9A, 0xC);
    // Entry 2: Kernel Data (0x10)
    encode_gdt_entry_32(&gdt[2], 0, 0xFFFFF, 0x92, 0xC);
    // Entry 3: User Code (0x18)
    encode_gdt_entry_32(&gdt[3], 0, 0xFFFFF, 0xFA, 0xC);
    // Entry 4: User Data (0x20)
    encode_gdt_entry_32(&gdt[4], 0, 0xFFFFF, 0xF2, 0xC);

    gdt_flush((uint32_t)&gdt_pointer);
}

void encode_gdt_entry_32(struct gdt_descriptor_32* entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) 
{
    entry->limit_low = (limit & 0xFFFF);
    entry->base_low = (base & 0xFFFF);
    entry->base_middle = (base >> 16) & 0xFF;
    entry->access_byte = access;
    entry->flags_limit = ((flags & 0x0F) << 4) | ((limit >> 16) & 0x0F);
    entry->base_high = (base >> 24) & 0xFF;
}

void encode_gdt_entry_64(struct gdt_descriptor_64* entry, uint64_t base, uint32_t limit, uint8_t access, uint8_t flags) 
{
    entry->limit_low = (limit & 0xFFFF);
    entry->base_low = (base & 0xFFFF);
    entry->base_middle = (base >> 16) & 0xFF;
    entry->access_byte = access;
    entry->flags_limit = ((flags & 0x0F) << 4) | ((limit >> 16) & 0x0F);
    entry->base_high = (base >> 24) & 0xFF;
    entry->base_highest = (base >> 32) & 0xFFFFFFFF;
    entry->reserved = 0;
}