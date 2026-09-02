#ifndef IO_H
#define IO_H

#include "types.h"

// Out 8
static inline void outb(ushort port, uchar val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Out 16
static inline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}


// In 8
static inline uchar inb(ushort port)
{
    uchar val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

// In 16
static inline uint16_t inw(uint16_t port)
{
    ushort val;
    __asm__ volatile ("inw %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

// Utils

static inline void io_wait(void)
{
    __asm__ volatile ("outb %%al, $0x80" : : "a"(0));
}

#endif