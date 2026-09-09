#include <stdbool.h>
#include "serial.h"
#include "utilities/iolayer.h"
#include "common_headers/io.h"
#include "utilities/format.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h> 

/*

    Majorly "inspired" (hemm copy pasted and adapted) by: https://github.com/brainboxdotcc/retro-rocket/blob/master/src/serial.c

*/

void serial_wait(uint16_t port)
{
    while ((inb(port + 5) & 0x20) == 0);
}

void serial_init_port(uint16_t port)
{
    print_fmt("Initialising serial {x}", port);
	outb(port + 1, 0x00); // Disable interrupts
	outb(port + 3, 0x80); // Enable DLAB (set baud rate divisor)
	outb(port + 0, 0x03); // Divisor low byte (38400 baud)
	outb(port + 1, 0x00); // Divisor high byte
	outb(port + 3, 0x03); // 8 bits, no parity, one stop bit
	outb(port + 2, 0xC7); // Enable FIFO, clear them, 14-byte threshold
	outb(port + 4, 0x0B); // IRQs enabled, RTS/DSR set
    print_fmt("Initialising serial {x} done!", port);
}

void serial_putc(uint16_t port, char c) 
{
	if (c == '\n') 
    {
		serial_putc(port, '\r'); // CRLF for terminals
	}
	serial_wait(port);
	outb(port, (uint8_t)c);
}

void serial_write_port(uint16_t port, const char *s) 
{
	while (*s) {
		serial_putc(port, *s++);
	}
}

void serial_print_port(uint16_t port, const char *fmt, ...) 
{
	va_list ap;
	va_start(ap, fmt);
	va_list ap_copy;
	__builtin_va_copy(ap_copy, ap);
	int len = format_vsnprintf(NULL, 0, fmt, ap_copy);
	va_end(ap_copy);
	if (len <= 0) 
    {
		va_end(ap);
		return;
	}
	char buf[len + 1];
	format_vsnprintf(buf, len + 1, fmt, ap);
	va_end(ap);
	serial_write_port(port, buf);
}