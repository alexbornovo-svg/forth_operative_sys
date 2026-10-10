#ifndef ULIB_H
#define ULIB_H

#include <stdint.h>
#include <stddef.h>

#define SYS_EXIT    0x00
#define SYS_WRITE   0x01
#define SYS_READ    0x02
#define SYS_CLEAN   0x03

static inline int sys_write(const char *buf, uint32_t len)
{
    int ret;

    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(SYS_WRITE), "b"(buf), "c"(len) : "memory");
    return ret;
}

static inline int sys_read(char *buf, uint32_t len)
{
    int ret;

    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(SYS_READ), "b"(buf), "c"(len) : "memory");
    return ret;
}

static inline int sys_screen_clean()
{
    int ret;

    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(SYS_CLEAN) : "memory");
}

static inline void sys_exit(int code)
{
    __asm__ volatile("int $0x80" : : "a"(SYS_EXIT), "b"(code));

    for (;;)
    {
    }
}

size_t strlen(const char *s);
int strcmp(const char *a, const char *b);
void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *dst, int value, size_t n);
void put_char(char c);
void print(const char *s);
void print_int(int value);
int read_line(char *buf, int max);

#endif