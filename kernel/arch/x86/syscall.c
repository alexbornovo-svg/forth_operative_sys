#include "syscall.h"
#include "isr.h"
#include "paging.h"

#include "utilities/iolayer.h"
#include "drivers/vga.h"

#include <stdbool.h>

#define READ_MAX 256

extern void usermode_return(void);

static bool user_range_ok(uint32_t addr, uint32_t len)
{
    if (addr < USER_BASE)
    {
        return false;
    }

    if (len > USER_REGION_SIZE)
    {
        return false;
    }

    return (addr - USER_BASE) <= (USER_REGION_SIZE - len);
}

static void syscall_handler(registers_t *regs)
{
    switch (regs->eax)
    {
        case SYS_EXIT:
        {
            print_fmt("{0,2}[USER]{0,F} process exited with code %d", (int)regs->ebx);
            usermode_return();
            break;
        }
        case SYS_WRITE:
        {
            uint32_t addr = regs->ebx;
            uint32_t len = regs->ecx;

            if (!user_range_ok(addr, len))
            {
                regs->eax = (uint32_t)-1;
                break;
            }

            console_write((const char *)addr, (int)len);
            regs->eax = len;
            break;
        }
        case SYS_READ:
        {
            uint32_t addr = regs->ebx;
            uint32_t len = regs->ecx;
            char kbuf[READ_MAX];

            if (len == 0 || !user_range_ok(addr, len))
            {
                regs->eax = (uint32_t)-1;
                break;
            }

            if (len > READ_MAX)
            {
                len = READ_MAX;
            }

            __asm__ volatile("sti");
            input_get(0, kbuf, (int)len);

            uint32_t n = 0;

            while (n < len - 1 && kbuf[n] != '\0')
            {
                n++;
            }

            char *dst = (char *)addr;

            for (uint32_t i = 0; i < n; i++)
            {
                dst[i] = kbuf[i];
            }

            dst[n] = '\n';
            n++;
            regs->eax = n;
            break;
        }
        case SYS_CLEAN:
        {
            vga_clean_screen();
            set_line(0);
            regs->eax = 0;
            break;
        }
        default:
        {
            regs->eax = (uint32_t)-1;
            break;
        }
    }
}

void syscall_init(void)
{
    register_interrupt_handler(128, syscall_handler);
}