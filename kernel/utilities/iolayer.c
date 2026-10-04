#include "drivers/vga.h"
#include "drivers/ps2kbd.h"
#include "common_headers/types.h"
#include "iolayer.h"
#include <stdbool.h>
#include "common_headers/io.h"
#include <stdarg.h>

#define TAG_BUF_SIZE 16
#define FMT_BUF_SIZE 256

typedef struct
{
    char *buf;
    int idx;
    int max_len;
} fmt_out_t;

static int line = 0;
static int cursor_column = 0;

static uint8_t hex_char_to_val(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    return 0;
}

static uint8_t parse_color_tag(const char *buf, int len)
{
    uint8_t bg = BLACK;
    uint8_t fg = WHITE;
    int comma_pos = -1;

    for (int i = 0; i < len; i++)
    {
        if (buf[i] == ',')
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

static void io_newline(int *column, int *current_line)
{
    *column = 0;
    (*current_line)++;
    if (*current_line >= VGA_HEIGHT)
    {
        vga_scroll();
        *current_line = VGA_HEIGHT - 1;
    }
}

static void io_advance(int *column, int *current_line)
{
    (*column)++;
    if (*column >= VGA_WIDTH)
    {
        io_newline(column, current_line);
    }
}

static void sync_cursor(int column, int row)
{
    uint16_t pos = (uint16_t)(row * VGA_WIDTH + column);

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void console_write(const char *buf, int len)
{
    uint8_t color = vga_entry_color(WHITE, BLACK);
    int current_column = cursor_column;
    int current_line = line;

    for (int i = 0; i < len; i++)
    {
        char c = buf[i];

        if (c == '\n')
        {
            io_newline(&current_column, &current_line);
            continue;
        }

        if (c == '\r')
        {
            current_column = 0;
            continue;
        }

        if (c == '\b')
        {
            if (current_column > 0)
            {
                current_column--;
                vga_put_char(' ', color, current_column, current_line);
            }
            continue;
        }

        vga_put_char(c, color, current_column, current_line);
        io_advance(&current_column, &current_line);
    }

    line = current_line;
    cursor_column = current_column;
    sync_cursor(current_column, current_line);
}

void print_line(const char *msg)
{
    uint8_t current_color = vga_entry_color(WHITE, BLACK);
    bool parsing_color_entry = false;
    char tag_buf[TAG_BUF_SIZE];
    int tag_idx = 0;
    int column = 0;
    int current_line = line;

    if (cursor_column != 0)
    {
        io_newline(&column, &current_line);
    }

    for (int i = 0; msg[i] != '\0'; i++)
    {
        if (!parsing_color_entry && msg[i] == '{')
        {
            parsing_color_entry = true;
            tag_idx = 0;
            continue;
        }

        if (parsing_color_entry)
        {
            if (msg[i] == '}')
            {
                tag_buf[tag_idx] = '\0';
                current_color = parse_color_tag(tag_buf, tag_idx);
                parsing_color_entry = false;
            }
            else if (tag_idx < TAG_BUF_SIZE - 1)
            {
                tag_buf[tag_idx++] = msg[i];
            }
            continue;
        }

        if (msg[i] == '\n')
        {
            io_newline(&column, &current_line);
            continue;
        }

        vga_put_char(msg[i], current_color, column, current_line);
        io_advance(&column, &current_line);
    }

    io_newline(&column, &current_line);
    line = current_line;
    cursor_column = 0;
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
    cursor_column = 0;
}

static void fmt_putc(fmt_out_t *out, char c)
{
    if (out->idx < out->max_len - 1)
    {
        out->buf[out->idx++] = c;
    }
}

static void fmt_puts(fmt_out_t *out, const char *s)
{
    if (s == 0)
    {
        s = "(null)";
    }

    while (*s != '\0')
    {
        fmt_putc(out, *s);
        s++;
    }
}

static void fmt_put_uint(fmt_out_t *out, unsigned int value, unsigned int base, bool upper, int width, bool zero_pad)
{
    char tmp[32];
    int tmp_len = 0;

    if (value == 0)
    {
        tmp[tmp_len++] = '0';
    }

    while (value > 0)
    {
        unsigned int digit = value % base;

        if (digit < 10)
        {
            tmp[tmp_len++] = (char)('0' + digit);
        }
        else
        {
            tmp[tmp_len++] = (char)((upper ? 'A' : 'a') + (digit - 10));
        }

        value /= base;
    }

    for (int pad = tmp_len; pad < width; pad++)
    {
        fmt_putc(out, zero_pad ? '0' : ' ');
    }

    while (tmp_len > 0)
    {
        tmp_len--;
        fmt_putc(out, tmp[tmp_len]);
    }
}

static void fmt_put_int(fmt_out_t *out, int value, int width, bool zero_pad)
{
    if (value < 0)
    {
        fmt_putc(out, '-');
        fmt_put_uint(out, 0u - (unsigned int)value, 10, false, width > 0 ? width - 1 : 0, zero_pad);
    }
    else
    {
        fmt_put_uint(out, (unsigned int)value, 10, false, width, zero_pad);
    }
}

int format_string(char *buf, int max_len, const char *fmt, va_list args)
{
    fmt_out_t out = { buf, 0, max_len };

    for (int i = 0; fmt[i] != '\0'; i++)
    {
        if (fmt[i] != '%')
        {
            fmt_putc(&out, fmt[i]);
            continue;
        }

        i++;

        bool zero_pad = false;
        int width = 0;

        if (fmt[i] == '0')
        {
            zero_pad = true;
            i++;
        }

        while (fmt[i] >= '0' && fmt[i] <= '9')
        {
            width = width * 10 + (fmt[i] - '0');
            i++;
        }

        if (fmt[i] == '\0')
        {
            break;
        }

        switch (fmt[i])
        {
            case 'd':
            {
                fmt_put_int(&out, va_arg(args, int), width, zero_pad);
                break;
            }
            case 'u':
            {
                fmt_put_uint(&out, va_arg(args, unsigned int), 10, false, width, zero_pad);
                break;
            }
            case 'x':
            {
                fmt_put_uint(&out, va_arg(args, unsigned int), 16, false, width, zero_pad);
                break;
            }
            case 'X':
            {
                fmt_put_uint(&out, va_arg(args, unsigned int), 16, true, width, zero_pad);
                break;
            }
            case 'p':
            {
                fmt_puts(&out, "0x");
                fmt_put_uint(&out, (unsigned int)va_arg(args, void *), 16, false, 8, true);
                break;
            }
            case 's':
            {
                fmt_puts(&out, va_arg(args, const char *));
                break;
            }
            case 'c':
            {
                fmt_putc(&out, (char)va_arg(args, int));
                break;
            }
            case '%':
            {
                fmt_putc(&out, '%');
                break;
            }
            default:
            {
                fmt_putc(&out, '%');
                fmt_putc(&out, fmt[i]);
                break;
            }
        }
    }

    if (max_len > 0)
    {
        buf[out.idx] = '\0';
    }

    return out.idx;
}

void print_fmt(const char *fmt, ...)
{
    char buf[FMT_BUF_SIZE];
    va_list args;

    va_start(args, fmt);
    format_string(buf, FMT_BUF_SIZE, fmt, args);
    va_end(args);

    print_line(buf);
}

void set_line(int new_line_value)
{
    if (new_line_value >= 0 && new_line_value < VGA_HEIGHT)
    {
        line = new_line_value;
    }
}

int get_line(void)
{
    return line;
}