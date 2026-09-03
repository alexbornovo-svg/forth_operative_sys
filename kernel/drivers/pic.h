#ifndef PIC_H
#define PIC_H

#include "common_headers/types.h"

void pic_remap();

void pic_send_eoi(uint8_t irq);

#endif