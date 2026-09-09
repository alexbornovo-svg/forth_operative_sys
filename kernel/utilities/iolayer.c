#include "drivers/vga.h"
#include "drivers/ps2kbd.h"
#include "common_headers/char_utils.h"
#include "iolayer.h"
#include <stdbool.h>
#include <stdarg.h>

int line = 0;

static uint8_t hex_char_to_val(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

static uint8_t parse_color_tag(const char *buf, int len)
{
    uint8_t bg = BLACK;
    uint8_t fg = WHITE;

    int comma_pos = -1;
    for (int i = 0; i < len; i++)
    {
        if (char_cmp(buf[i], ',') == 0)
        {
            comma_pos = i;
            break;
        }
    }

    if (comma_pos != -1)
    {
        if (comma_pos > 0)
        {
            bg = hex_char_to_val(buf[0]);
        }
        if (comma_pos + 1 < len)
        {
            fg = hex_char_to_val(buf[comma_pos + 1]);
        }
    }
    else if (len >= 2)
    {
        bg = hex_char_to_val(buf[0]);
        fg = hex_char_to_val(buf[1]);
    }

    return vga_entry_color(fg, bg);
}

static void io_advance(int *column, int *current_line)
{
    (*column)++;
    if (*column >= VGA_WIDTH)
    {
        *column = 0;
        (*current_line)++;
        if (*current_line >= VGA_HEIGHT)
        {
            vga_scroll();
            *current_line = VGA_HEIGHT - 1;
        }
    }
}

void print_line(const char *msg)
{
    uint8_t current_color = vga_entry_color(WHITE, BLACK);
    bool parsing_color_entry = false;

    char tag_buf[16];
    int tag_idx = 0;

    int column = 0;
    int current_line = line;

    for (int i = 0; msg[i] != '\0'; i++)
    {
        if (!parsing_color_entry && char_cmp(msg[i], '{') == 0)
        {
            parsing_color_entry = true;
            tag_idx = 0;
            continue;
        }

        if (parsing_color_entry)
        {
            if (char_cmp(msg[i], '}') == 0)
            {
                tag_buf[tag_idx] = '\0';
                current_color = parse_color_tag(tag_buf, tag_idx);
                parsing_color_entry = false;
            }
            else if (tag_idx < (int)sizeof(tag_buf) - 1)
            {
                tag_buf[tag_idx++] = msg[i];
            }
            continue;
        }

        if (msg[i] == '\n')
        {
            column = 0;
            current_line++;
            if (current_line >= VGA_HEIGHT)
            {
                vga_scroll();
                current_line = VGA_HEIGHT - 1;
            }
            continue;
        }

        vga_put_char(msg[i], current_color, column, current_line);
        io_advance(&column, &current_line);
    }

    line = current_line + 1;
    if (line >= VGA_HEIGHT)
    {
        vga_scroll();
        line = VGA_HEIGHT - 1;
    }
}

void input_get(const char *prompt, char *dest, int max_len)
{
    if (prompt != 0)
    {
        print_line(prompt);
    }

    uint16_t cursor_pos = get_cursor_position();
    int current_x = cursor_pos % VGA_WIDTH;
    int current_y = cursor_pos / VGA_WIDTH;

    kbd_gets(dest, max_len, &current_x, &current_y);

    line = current_y;
}

static void fmt_putc(char *buf, int *idx, int max_len, char c)
{
    if (*idx < max_len - 1)
    {
        buf[*idx] = c;
        (*idx)++;
    }
}

static void fmt_puts(char *buf, int *idx, int max_len, const char *s)
{
    while (*s != '\0')
    {
        fmt_putc(buf, idx, max_len, *s);
        s++;
    }
}

static void fmt_put_uint(char *buf, int *idx, int max_len, unsigned int value, int base, bool upper)
{
    char tmp[32];
    int tmp_len = 0;

    if (value == 0)
    {
        tmp[tmp_len++] = '0';
    }

    while (value > 0)
    {
        int digit = value % base;
        char c;

        if (digit < 10)
        {
            c = '0' + digit;
        }
        else
        {
            c = (upper ? 'A' : 'a') + (digit - 10);
        }

        tmp[tmp_len++] = c;
        value /= base;
    }

    while (tmp_len > 0)
    {
        tmp_len--;
        fmt_putc(buf, idx, max_len, tmp[tmp_len]);
    }
}

static void fmt_put_int(char *buf, int *idx, int max_len, int value)
{
    if (value < 0)
    {
        fmt_putc(buf, idx, max_len, '-');
        fmt_put_uint(buf, idx, max_len, (unsigned int)(-value), 10, false);
    }
    else
    {
        fmt_put_uint(buf, idx, max_len, (unsigned int)value, 10, false);
    }
}

void print_fmt(const char *fmt, ...)
{
    char buf[256];
    int idx = 0;

    va_list args;
    va_start(args, fmt);

    for (int i = 0; fmt[i] != '\0'; i++)
    {
        if (fmt[i] != '%')
        {
            fmt_putc(buf, &idx, (int)sizeof(buf), fmt[i]);
            continue;
        }

        i++;

        if (fmt[i] == '\0')
        {
            break;
        }

        switch (fmt[i])
        {
            case 'd':
            {
                int value = va_arg(args, int);
                fmt_put_int(buf, &idx, (int)sizeof(buf), value);
                break;
            }
            case 'u':
            {
                unsigned int value = va_arg(args, unsigned int);
                fmt_put_uint(buf, &idx, (int)sizeof(buf), value, 10, false);
                break;
            }
            case 'x':
            {
                unsigned int value = va_arg(args, unsigned int);
                fmt_put_uint(buf, &idx, (int)sizeof(buf), value, 16, false);
                break;
            }
            case 'X':
            {
                unsigned int value = va_arg(args, unsigned int);
                fmt_put_uint(buf, &idx, (int)sizeof(buf), value, 16, true);
                break;
            }
            case 's':
            {
                const char *s = va_arg(args, const char *);
                fmt_puts(buf, &idx, (int)sizeof(buf), s);
                break;
            }
            case 'c':
            {
                char c = (char)va_arg(args, int);
                fmt_putc(buf, &idx, (int)sizeof(buf), c);
                break;
            }
            case '%':
            {
                fmt_putc(buf, &idx, (int)sizeof(buf), '%');
                break;
            }
            default:
            {
                fmt_putc(buf, &idx, (int)sizeof(buf), '%');
                fmt_putc(buf, &idx, (int)sizeof(buf), fmt[i]);
                break;
            }
        }
    }

    va_end(args);

    buf[idx] = '\0';

    print_line(buf);
}

void set_line(int new_line_value)
{
    if (new_line_value >= 0 && new_line_value < 25)
    {
        line = new_line_value;
    }
}