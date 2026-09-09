#include <stdint.h>
#include "drivers/vga.h" 
#include "arch/x86/gdt.h"
#include "arch/x86/idt.h"
#include "arch/x86/isr.h"
#include "drivers/pic.h"
#include "drivers/pit.h"
#include "drivers/ps2kbd.h"
#include "drivers/ata_pio.h"
#include "utilities/iolayer.h"
#include "drivers/serial.h"
#include "common_headers/char_utils.h"

void kernel_main()
{
    vga_clean_screen();

    // Init
    gdt_init();
    idt_init();
    isr_init();

    pic_remap();

    ata_pio_init();
    kbd_init();

    pit_init(100);

    serial_init_port(COM_1);

    __asm__ __volatile__("sti");

    // Best message ever
    print_line("{0,2}[KERNEL]{0,F} - Kernel loaded successfully");
    line = 2;

    kbd_set_layout(&layout_it);
    enable_cursor(14, 15);

    set_line(3);

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
    }
}