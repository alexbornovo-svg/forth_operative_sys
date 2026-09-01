#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define WIDTH 80
#define HEIGHT 25

#define BLACK 0x0
#define GREEN 0x2
#define RED   0x4
#define GREY  0x8
#define WHITE 0xF

uint8_t vga_entry_color(uint8_t fg, uint8_t bg);
uint16_t vga_entry(unsigned char uc, uint8_t color);

void vga_put_char(char c, uint8_t color, int x, int y);
int vga_put_chars(char* c, uint8_t color, int line);

#endif