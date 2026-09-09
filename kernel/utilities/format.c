#include "format.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    char* buf;
    size_t size;
    size_t written;
} format_sink_t;

static void sink_putc(format_sink_t* sink, char c)
{
    if (sink->buf != 0 && sink->written + 1 < sink->size)
    {
        sink->buf[sink->written] = c;
    }
    sink->written++;
}

static void sink_puts(format_sink_t* sink, const char* s)
{
    while (*s != '\0')
    {
        sink_putc(sink, *s++);
    }
}

static void sink_put_uint(format_sink_t* sink, uint32_t value, uint32_t base, bool uppercase)
{
    char digits[32];
    int idx = 0;

    if (value == 0)
    {
        sink_putc(sink, '0');
        return;
    }

    while (value > 0)
    {
        uint32_t digit = value % base;
        char c;
        if (digit < 10)
        {
            c = (char)('0' + digit);
        }
        else
        {
            c = (char)((uppercase ? 'A' : 'a') + (digit - 10));
        }
        digits[idx++] = c;
        value /= base;
    }

    while (idx > 0)
    {
        sink_putc(sink, digits[--idx]);
    }
}

static void sink_put_int(format_sink_t* sink, int32_t value)
{
    if (value < 0)
    {
        sink_putc(sink, '-');
        sink_put_uint(sink, (uint32_t)(-(value + 1)) + 1, 10, false);
        return;
    }
    sink_put_uint(sink, (uint32_t)value, 10, false);
}

int format_vsnprintf(char* buf, size_t size, const char* fmt, va_list ap)
{
    format_sink_t sink;
    sink.buf = buf;
    sink.size = size;
    sink.written = 0;

    for (int i = 0; fmt[i] != '\0'; i++)
    {
        if (fmt[i] != '%')
        {
            sink_putc(&sink, fmt[i]);
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
                int value = va_arg(ap, int);
                sink_put_int(&sink, value);
                break;
            }
            case 'u':
            {
                unsigned int value = va_arg(ap, unsigned int);
                sink_put_uint(&sink, value, 10, false);
                break;
            }
            case 'x':
            {
                unsigned int value = va_arg(ap, unsigned int);
                sink_put_uint(&sink, value, 16, false);
                break;
            }
            case 'X':
            {
                unsigned int value = va_arg(ap, unsigned int);
                sink_put_uint(&sink, value, 16, true);
                break;
            }
            case 'c':
            {
                char value = (char)va_arg(ap, int);
                sink_putc(&sink, value);
                break;
            }
            case 's':
            {
                const char* value = va_arg(ap, const char*);
                if (value == 0)
                {
                    sink_puts(&sink, "(null)");
                }
                else
                {
                    sink_puts(&sink, value);
                }
                break;
            }
            case '%':
            {
                sink_putc(&sink, '%');
                break;
            }
            default:
            {
                sink_putc(&sink, '%');
                sink_putc(&sink, fmt[i]);
                break;
            }
        }
    }

    if (sink.buf != 0 && sink.size > 0)
    {
        size_t term = sink.written < sink.size ? sink.written : sink.size - 1;
        sink.buf[term] = '\0';
    }

    return (int)sink.written;
}

int format_snprintf(char* buf, size_t size, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int result = format_vsnprintf(buf, size, fmt, ap);
    va_end(ap);
    return result;
}