#include <stdint.h>

#include "arch/x86/gdt.h"
#include "arch/x86/idt.h"
#include "arch/x86/isr.h"
#include "arch/x86/paging.h"
#include "arch/x86/syscall.h"
#include "arch/x86/usermode.h"

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
extern uint8_t user_forth_start[];
extern uint8_t user_forth_end[];

static void run_user_image(const uint8_t *start, const uint8_t *end)
{
    uint32_t size = (uint32_t)(end - start);

    if (size > USER_REGION_SIZE - 0x10000)
    {
        print_line("{0,C}[KERNEL]{0,F} - user image too large");
        return;
    }

    void *zero_dest = (void *)USER_BASE;
    uint32_t zero_count = USER_REGION_SIZE;
    void *copy_dest = (void *)USER_BASE;
    const uint8_t *copy_src = start;
    uint32_t copy_count = size;

    __asm__ volatile("cld; rep stosb" : "+D"(zero_dest), "+c"(zero_count) : "a"(0) : "memory");
    __asm__ volatile("cld; rep movsb" : "+D"(copy_dest), "+S"(copy_src), "+c"(copy_count) : : "memory");

    enter_usermode(USER_BASE, USER_BASE + USER_REGION_SIZE - 16);
}

void kernel_main(void)
{
    vga_clean_screen();

    gdt_init();
    idt_init();
    isr_init();
    syscall_init();

    pic_remap();

    paging_init();
    paging_enable();

    ata_pio_init();
    kbd_init();

    pit_init(100);

    serial_init_port(COM_1);

    __asm__ __volatile__("sti");

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
            run_user_image(user_hello_start, user_hello_end);
        }
        else if (chars_cmp("forth", command_buffer))
        {
            run_user_image(user_forth_start, user_forth_end);
        }
    }
}