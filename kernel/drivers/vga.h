#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define BLACK 0x0
#define GREEN 0x2
#define RED   0x4
#define GREY  0x8
#define WHITE 0xF

uint8_t vga_entry_color(uint8_t fg, uint8_t bg);
uint16_t vga_entry(unsigned char uc, uint8_t color);

void vga_put_char(char c, uint8_t color, int x, int y);
int vga_put_chars(char* c, uint8_t color, int line);

// Cursor

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
uint16_t get_cursor_position();
void update_cursor(int x, int y);

#endif