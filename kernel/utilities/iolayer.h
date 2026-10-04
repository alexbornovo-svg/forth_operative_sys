#ifndef IOLAYER_H
#define IOLAYER_H

#include <stdarg.h>

void print_line(const char *msg);
void print_fmt(const char *fmt, ...);
int format_string(char *buf, int max_len, const char *fmt, va_list args);
void input_get(const char *prompt, char *dest, int max_len);
void set_line(int new_line_value);
int get_line(void);
void console_write(const char *buf, int len);

#endif