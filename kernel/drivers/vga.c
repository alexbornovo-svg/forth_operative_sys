#include "vga.h"

volatile uint16_t* vga_buffer = (volatile uint16_t*) 0xB8000;

uint8_t vga_entry_color(uint8_t fg, uint8_t bg) 
{
    return fg | (bg << 4);
}

uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
    return (uint16_t) uc | ((uint16_t) color << 8);
}

void vga_put_char(char c, uint8_t color, int x, int y) 
{
    int index = y * WIDTH + x;
    vga_buffer[index] = vga_entry(c, color);
}

int vga_put_chars(char* c, uint8_t color, int line) 
{
    if (line < 0 || line >= HEIGHT) 
    {
        return -1;
    }

    int column = 0;

    for (int i = 0; c[i] != '\0' && column < WIDTH; i++) 
    {
        if (c[i] == '\n') 
        {
            line++;
        }

        vga_put_char(c[i], color, column, line);
        column++;
    }

    return line + 1;
}