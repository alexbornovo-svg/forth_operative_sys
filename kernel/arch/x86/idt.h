#ifndef IDT_H
#define IDT_H

#include "common_headers/types.h"

typedef struct 
{
    uint16_t isr_low;      // The lower 16 bits of the ISR's address
    uint16_t kernel_cs;    // The GDT code segment selector (e.g., 0x08)
    uint8_t  reserved;     // Set to 0
    uint8_t  attributes;   // Type and attributes (e.g., 0x8E for interrupt gate)
    uint16_t isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_32_t;

typedef struct
{
    uint16_t isr_low;      // The lower 16 bits of the ISR's address
    uint16_t kernel_cs;    // The GDT code segment selector
    uint8_t  ist;          // Bits 0..2 hold the Interrupt Stack Table offset (TSS), others 0
    uint8_t  attributes;   // Type and attributes (e.g., 0x8E)
    uint16_t isr_mid;      // The middle 16 bits of the ISR's address
    uint32_t isr_high;     // The higher 32 bits of the ISR's address
    uint32_t reserved;     // Set to 0
} __attribute__((packed)) idt_entry_64_t;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_32_t;

extern idt_entry_32_t idt_entries[256];

extern void idt_flush(uint32_t idt_ptr_addr);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_init(void);

#endif