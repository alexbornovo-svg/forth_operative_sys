#include <stdint.h>

#include "arch/x86/gdt.h"
#include "arch/x86/idt.h"
#include "arch/x86/isr.h"
#include "arch/x86/paging.h"
#include "arch/x86/syscall.h"

#include "drivers/pic.h"
#include "drivers/pit.h"
#include "drivers/ps2kbd.h"
#include "drivers/ata_pio.h"
#include "drivers/vga.h" 
#include "drivers/serial.h"
#include "drivers/speaker.h"

#include "utilities/iolayer.h"
#include "common_headers/char_utils.h"
#include "utilities/starter.h"

extern uint8_t user_hello_start[];
extern uint8_t user_hello_end[];

static void usermode_test(void)
{
    uint8_t *dest = (uint8_t *)USER_BASE;
    uint32_t size = (uint32_t)(user_hello_end - user_hello_start);

    for (uint32_t i = 0; i < size; i++)
    {
        dest[i] = user_hello_start[i];
    }

    enter_usermode(USER_BASE, USER_BASE + USER_REGION_SIZE - 16);
}

void kernel_main()
{
    vga_clean_screen();

    // Init
    gdt_init();
    idt_init();
    isr_init();
    syscall_init();

    pic_remap();

    ata_pio_init();
    kbd_init();

    pit_init(100);

    serial_init_port(COM_1);

    __asm__ __volatile__("sti");

    paging_init();
    paging_enable();

    vga_clean_screen();

    starter_run();
    

    // Best message ever
    print_line("{0,2}[KERNEL]{0,F} - Kernel loaded successfully");

    kbd_set_layout(&layout_it);
    enable_cursor(14, 15);

    char command_buffer[128];

    // Mini Shell Loop
    while (1) 
    {
        input_get("{0,F}os> ", command_buffer, sizeof(command_buffer));

        if (chars_cmp("cls", command_buffer))
        {
            vga_clean_screen();
            set_line(0);
        }
        else if (chars_cmp("usermode", command_buffer))
        {
            usermode_test();
        }
    }
}