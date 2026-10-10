#include "ulib.h"

size_t strlen(const char *s)
{
    size_t n = 0;

    while (s[n] != '\0')
    {
        n++;
    }

    return n;
}

int strcmp(const char *a, const char *b)
{
    while (*a != '\0' && *a == *b)
    {
        a++;
        b++;
    }

    return (unsigned char)*a - (unsigned char)*b;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *d = dst;
    const unsigned char *s = src;

    for (size_t i = 0; i < n; i++)
    {
        d[i] = s[i];
    }

    return dst;
}

void *memset(void *dst, int value, size_t n)
{
    unsigned char *d = dst;

    for (size_t i = 0; i < n; i++)
    {
        d[i] = (unsigned char)value;
    }

    return dst;
}

void put_char(char c)
{
    sys_write(&c, 1);
}

void clear_screen()
{
    sys_screen_clean();
}

void print(const char *s)
{
    sys_write(s, (uint32_t)strlen(s));
}

void print_int(int value)
{
    char tmp[12];
    int n = 0;
    unsigned int u;

    if (value < 0)
    {
        put_char('-');
        u = 0u - (unsigned int)value;
    }
    else
    {
        u = (unsigned int)value;
    }

    if (u == 0)
    {
        tmp[n++] = '0';
    }

    while (u > 0)
    {
        tmp[n++] = (char)('0' + (u % 10));
        u /= 10;
    }

    while (n > 0)
    {
        n--;
        put_char(tmp[n]);
    }
}

int read_line(char *buf, int max)
{
    int n = sys_read(buf, (uint32_t)max);

    if (n <= 0)
    {
        buf[0] = '\0';
        return 0;
    }

    if (buf[n - 1] == '\n')
    {
        n--;
    }

    if (n >= max)
    {
        n = max - 1;
    }

    buf[n] = '\0';
    return n;
}