#include "idt.h"

idt_entry_32_t idt_entries[256];
static idt_ptr_32_t idt_ptr;

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();

extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();

extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();

extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].isr_low = (uint16_t)(base & 0xFFFF);
    idt_entries[num].isr_high = (uint16_t)((base >> 16) & 0xFFFF);
    idt_entries[num].kernel_cs = sel;
    idt_entries[num].reserved = 0;
    idt_entries[num].attributes = flags;
}

void idt_init()
{
    idt_ptr.limit = (sizeof(idt_entry_32_t) * 256) - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    for (int i = 0; i < 256; i++)
    {
        idt_set_gate(i, 0, 0, 0);
    }

    idt_flush((uint32_t)&idt_ptr);

    idt_set_gate(32, (uint32_t)irq0,  0x08, 0x8E);
    idt_set_gate(33, (uint32_t)irq1,  0x08, 0x8E);
    idt_set_gate(34, (uint32_t)irq2,  0x08, 0x8E);
    idt_set_gate(35, (uint32_t)irq3,  0x08, 0x8E);
    idt_set_gate(36, (uint32_t)irq4,  0x08, 0x8E);
    idt_set_gate(37, (uint32_t)irq5,  0x08, 0x8E);
    idt_set_gate(38, (uint32_t)irq6,  0x08, 0x8E);
    idt_set_gate(39, (uint32_t)irq7,  0x08, 0x8E);
    idt_set_gate(40, (uint32_t)irq8,  0x08, 0x8E);
    idt_set_gate(41, (uint32_t)irq9,  0x08, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);
}