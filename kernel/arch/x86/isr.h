#ifndef ISR_H
#define ISR_H

#include "common_headers/types.h"

typedef struct {
    uint32_t ds;                                     // Segment
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Saved reg and pusha 
    uint32_t int_no, err_code;                       // Interrupt and error code
    uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed)) registers_t;

typedef void (*isr_t)(registers_t*);

void isr_init(void);
void register_interrupt_handler(uint8_t n, isr_t handler);

#endif