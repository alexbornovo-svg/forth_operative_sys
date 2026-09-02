#include <stdint.h>
#include "drivers/vga.h" 
#include "arch/x86/gdt.h"
#include "arch/x86/idt.h"

void kernel_main()
{
    uint8_t standard_fbg = vga_entry_color(WHITE, BLACK);
    uint8_t green_fbg = vga_entry_color(GREEN, BLACK);

    vga_clean_screen();
    int line = 0;

    // initializing everything
    gdt_init();


    // the message i always want to see
    line = vga_put_chars("[KERNEL] - Kernel loaded succesfully", green_fbg, line);

    // Forth VM starts here

    while (1) {}
}