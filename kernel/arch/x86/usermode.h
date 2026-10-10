#ifndef USERMODE_H
#define USERMODE_H

#include "common_headers/types.h"

void enter_usermode(uint32_t entry, uint32_t user_stack);
void usermode_return(void);

#endif