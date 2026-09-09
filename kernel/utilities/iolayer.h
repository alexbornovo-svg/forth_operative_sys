#ifndef IO_LAYER_H
#define IO_LAYER_H

extern int line;

void print_line(const char *msg);

void input_get(const char *prompt, char *dest, int max_len);

void print_fmt(const char *fmt, ...);

void set_line(int new_line_value);

#endif