#include <stdint.h>
#include "drivers/vga.h" 
#include "arch/x86/gdt.h"
#include "arch/x86/idt.h"
#include "arch/x86/isr.h"
#include "drivers/pic.h"
#include "drivers/pit.h"
#include "drivers/ps2kbd.h"

void kernel_main()
{
    uint8_t green_fbg = vga_entry_color(GREEN, BLACK);
    uint8_t white_fbg = vga_entry_color(WHITE, BLACK);

    vga_clean_screen();

    // Init
    gdt_init();
    idt_init();
    isr_init();

    pic_remap();
    pic_remap();

    kbd_init();

    pit_init(100);

    __asm__ __volatile__("sti");

    // Best message ever
    int line = 0;
    vga_put_chars("[KERNEL] - Kernel loaded successfully\n", green_fbg, line);
    line = 2;

    kbd_set_layout(&layout_it);
    enable_cursor(14, 15);

    char command_buffer[128];
    int cursor_x = 0;
    int cursor_y = line;

    // Mini Shell Loop
    while (1) {

        vga_put_char('o', white_fbg, 0, cursor_y);
        vga_put_char('s', white_fbg, 1, cursor_y);
        vga_put_char('>', white_fbg, 2, cursor_y);
        vga_put_char(' ', white_fbg, 3, cursor_y);
        
        cursor_x = 4;
        update_cursor(cursor_x, cursor_y);

        kbd_gets(command_buffer, sizeof(command_buffer), &cursor_x, &cursor_y);
    }
}