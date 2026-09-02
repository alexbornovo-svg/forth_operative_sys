#include "gdt.h"

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