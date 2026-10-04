#ifndef GDT_H
#define GDT_H

#include "common_headers/types.h"

/*

I have placed here both a 32 bit and a 64 bit version for the GDT table. 

I'll probably just use the 32 bit version for the rest of my life, but yeah, i have the 64 bit if needed.

*/

struct gdt_descriptor_32 
{
    uint16_t limit_low;     // Bits 0-15 of segment limit
    uint16_t base_low;      // Bits 0-15 of base address
    uint8_t  base_middle;   // Bits 16-23 of base address
    uint8_t  access_byte;   // Access byte flags
    uint8_t  flags_limit;   // Top 4 bits are flags, bottom 4 bits are limit 16-19
    uint8_t  base_high;     // Bits 24-31 of base address
} __attribute__((packed));

struct gdt_descriptor_64 
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access_byte;
    uint8_t  flags_limit;
    uint8_t  base_high;
    uint32_t base_highest;  // Bits 32-63 of base address (System descriptors only)
    uint32_t reserved;      // Must be zero
} __attribute__((packed));

struct gdt_ptr_32 
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct tss_entry_32
{
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));

#endif