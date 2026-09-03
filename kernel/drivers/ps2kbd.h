#ifndef PS2_KBD_H
#define PS2_KBD_H

#include <stdint.h>
#include <stdbool.h>

#include "common_headers/io.h"
#include "common_headers/types.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

#define PIC1_COMMAND 0x20
#define PIC_EOI 0x20

#define KBD_SCANCODE_EXTENDED_PREFIX 0xE0
#define KBD_SCANCODE_RELEASE_MASK 0x80
#define KBD_SCANCODE_LSHIFT 0x2A
#define KBD_SCANCODE_RSHIFT 0x36

typedef struct 
{
    const char* normal;
    const char* shift;
} kbd_layout_t;

extern kbd_layout_t layout_us;
extern kbd_layout_t layout_it;

void kbd_init(void);
void kbd_set_layout(kbd_layout_t* new_layout);
char kbd_getc_nonblocking(void);
char scancode_to_ascii(uint8_t scancode);
void keyboard_handler(void);
void kbd_gets(char* dest, int max_len, int* current_x, int* current_y);

#endif