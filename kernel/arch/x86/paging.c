#include "paging.h"
#include "isr.h"
#include "utilities/iolayer.h"

#define MAPPED_TABLES 4

uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t page_tables[MAPPED_TABLES][1024] __attribute__((aligned(4096)));

static void page_fault_handler(registers_t *regs)
{
    uint32_t fault_addr = paging_fault_address();
    uint32_t err = regs->err_code;

    print_fmt("{0,C}PAGE FAULT{0,F} at %p eip=%p", (void *)fault_addr, (void *)regs->eip);
    print_fmt("%s | %s | %s%s%s",
              (err & 0x1) ? "protection violation" : "page not present",
              (err & 0x2) ? "write" : "read",
              (err & 0x4) ? "user" : "supervisor",
              (err & 0x8) ? " | reserved bit" : "",
              (err & 0x10) ? " | instruction fetch" : "");

    for (;;)
    {
        __asm__ volatile("cli; hlt");
    }
}

void paging_init()
{
    int i;
    int j;

    for (i = 0; i < 1024; i++)
    {
        page_directory[i] = 0x00000002;
    }

    for (i = 0; i < MAPPED_TABLES; i++)
    {
        uint32_t flags = PAGE_PRESENT | PAGE_WRITE;

        if (i == USER_TABLE)
        {
            flags |= PAGE_USER;
        }

        for (j = 0; j < 1024; j++)
        {
            uint32_t phys = ((uint32_t)i * 1024 + (uint32_t)j) * PAGE_SIZE;
            page_tables[i][j] = phys | flags;
        }

        page_directory[i] = ((uint32_t)page_tables[i]) | flags;
    }

    register_interrupt_handler(14, page_fault_handler);
}

void paging_enable()
{
    uint32_t cr0;

    __asm__ volatile("mov %0, %%cr3" : : "r"((uint32_t)page_directory) : "memory");
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0) : "memory");
}

int paging_map(uint32_t virt, uint32_t phys, uint32_t flags)
{
    uint32_t dir_index = virt >> 22;
    uint32_t table_index = (virt >> 12) & 0x3FF;

    if (dir_index >= MAPPED_TABLES)
    {
        return -1;
    }

    page_tables[dir_index][table_index] = (phys & 0xFFFFF000) | (flags & 0xFFF) | PAGE_PRESENT;
    __asm__ volatile("invlpg (%0)" : : "r"(virt) : "memory");
    return 0;
}

uint32_t paging_fault_address()
{
    uint32_t addr;

    __asm__ volatile("mov %%cr2, %0" : "=r"(addr));
    return addr;
}