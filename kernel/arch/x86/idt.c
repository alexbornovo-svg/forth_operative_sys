#include "idt.h"

idt_entry_32_t idt_entries[256];
static idt_ptr_32_t idt_ptr;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].isr_low = (uint16_t)(base & 0xFFFF);
    idt_entries[num].isr_high = (uint16_t)((base >> 16) & 0xFFFF);
    idt_entries[num].kernel_cs = sel;
    idt_entries[num].reserved = 0;
    idt_entries[num].attributes = flags;
}

void idt_init(void)
{
    idt_ptr.limit = (sizeof(idt_entry_32_t) * 256) - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    for (int i = 0; i < 256; i++)
    {
        idt_set_gate(i, 0, 0, 0);
    }

    idt_flush((uint32_t)&idt_ptr);
}