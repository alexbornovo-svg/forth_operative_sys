#ifndef PAGING_H
#define PAGING_H

#include "common_headers/types.h"

#define PAGE_SIZE 4096
#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4

#define USER_TABLE 2
#define USER_BASE 0x00800000
#define USER_REGION_SIZE 0x00400000

extern uint32_t page_directory[1024] __attribute__((aligned(4096)));

void paging_init();
void paging_enable();
int paging_map(uint32_t virt, uint32_t phys, uint32_t flags);
uint32_t paging_fault_address();

#endif