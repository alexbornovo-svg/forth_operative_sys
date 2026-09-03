#ifndef PIT_H
#define PIT_H

#include "common_headers/types.h"

#define PIT_CHANNEL_0 0x40
#define PIT_COMMAND 0x43
#define PIT_BASE_FREQ 1193182

// Init pit at desired freq. (100 Hz -> 100 interrupts evey sec)
void pit_init(uint32_t frequency);

uint32_t pit_get_ticks();

#endif