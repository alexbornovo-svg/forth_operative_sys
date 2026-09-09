#ifndef SERIAL_H
#define SERIAL_H

#include <stdbool.h>
#include <stdint.h>

#define COM_1 0x3F8
#define COM_2 0x2F8
#define COM_3 0x3E8
#define COM_4 0x2E8

void serial_init_port(uint16_t port);
void serial_wait_port(uint16_t port);
void serial_write_port(uint16_t port, const char *s);
void serial_print_port(uint16_t port, const char *fmt, ...);

#endif