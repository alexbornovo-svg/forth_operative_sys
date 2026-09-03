#include "ps2kbd.h"
#include "vga.h"
#include "arch/x86/isr.h"
#include "pic.h"
#include "misc/kbd_it_IT.h"
#include "misc/kbd_us_US.h"

#define KEYBOARD_IRQ_LINE 1
#define KEYBOARD_IDT_VECTOR 33
#define PIC1_DATA 0x21

#define KBD_BUFFER_SIZE 256

static char kbd_buffer[KBD_BUFFER_SIZE];
static volatile uint16_t buffer_head = 0;
static volatile uint16_t buffer_tail = 0;

kbd_layout_t layout_us = {kbd_us_layout, kbd_us_layout_shift};
kbd_layout_t layout_it = {kbd_it_layout, kbd_it_layout_shift};

static kbd_layout_t* active_layout = &layout_it;
static bool shift_pressed = false;
static bool extended_prefix = false;

static void kbd_irq_handler(registers_t* regs)
{
    (void)regs;
    keyboard_handler();
}

void kbd_init(void)
{
    register_interrupt_handler(KEYBOARD_IDT_VECTOR, kbd_irq_handler);

    uint8_t mask = inb(PIC1_DATA);
    mask &= ~(1 << KEYBOARD_IRQ_LINE);
    outb(PIC1_DATA, mask);
}

void kbd_set_layout(kbd_layout_t* new_layout)
{
    if (new_layout != 0)
    {
        active_layout = new_layout;
    }
}

static void kbd_buffer_push(char c)
{
    uint16_t next = (buffer_head + 1) % KBD_BUFFER_SIZE;
    if (next != buffer_tail)
    {
        kbd_buffer[buffer_head] = c;
        buffer_head = next;
    }
}

char kbd_getc_nonblocking(void)
{
    if (buffer_head == buffer_tail)
    {
        return 0;
    }
    char c = kbd_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % KBD_BUFFER_SIZE;
    return c;
}

char scancode_to_ascii(uint8_t scancode)
{
    if (scancode == KBD_SCANCODE_EXTENDED_PREFIX)
    {
        extended_prefix = true;
        return 0;
    }

    if (extended_prefix)
    {
        extended_prefix = false;
        return 0;
    }

    if (scancode & KBD_SCANCODE_RELEASE_MASK)
    {
        uint8_t released_code = scancode & 0x7F;
        if (released_code == KBD_SCANCODE_LSHIFT || released_code == KBD_SCANCODE_RSHIFT)
        {
            shift_pressed = false;
        }
        return 0;
    }

    if (scancode == KBD_SCANCODE_LSHIFT || scancode == KBD_SCANCODE_RSHIFT)
    {
        shift_pressed = true;
        return 0;
    }

    if (scancode < 128 && active_layout != 0)
    {
        const char* current_map = shift_pressed ? active_layout->shift : active_layout->normal;
        return current_map[scancode];
    }

    return 0;
}

void keyboard_handler(void)
{
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    char c = scancode_to_ascii(scancode);
    if (c != 0)
    {
        kbd_buffer_push(c);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}

void kbd_gets(char* dest, int max_len, int* current_x, int* current_y)
{
    int i = 0;

    while (i < max_len - 1)
    {
        char c = kbd_getc_nonblocking();
        if (c == 0)
        {
            asm volatile("hlt");
            continue;
        }

        if (c == '\n')
        {
            dest[i] = '\0';
            (*current_y)++;
            *current_x = 0;
            if (*current_y >= VGA_HEIGHT)
            {
                vga_scroll();
                *current_y = VGA_HEIGHT - 1;
            }
            update_cursor(*current_x, *current_y);
            return;
        }

        if (c == '\b')
        {
            if (i > 0)
            {
                i--;
                if (*current_x > 0)
                {
                    (*current_x)--;
                }
                else if (*current_y > 0)
                {
                    (*current_y)--;
                    *current_x = VGA_WIDTH - 1;
                }
                vga_put_char(' ', vga_entry_color(WHITE, BLACK), *current_x, *current_y);
                update_cursor(*current_x, *current_y);
            }
            continue;
        }

        dest[i++] = c;
        vga_put_char(c, vga_entry_color(WHITE, BLACK), *current_x, *current_y);
        (*current_x)++;

        if (*current_x >= VGA_WIDTH)
        {
            *current_x = 0;
            (*current_y)++;
            if (*current_y >= VGA_HEIGHT)
            {
                vga_scroll();
                *current_y = VGA_HEIGHT - 1;
            }
        }
        update_cursor(*current_x, *current_y);
    }

    dest[i] = '\0';
}