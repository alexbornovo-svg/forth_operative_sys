#ifndef FORMAT_H
#define FORMAT_H

#include <stdarg.h>
#include <stddef.h>

int format_vsnprintf(char* buf, size_t size, const char* fmt, va_list ap);
int format_snprintf(char* buf, size_t size, const char* fmt, ...);

#endif