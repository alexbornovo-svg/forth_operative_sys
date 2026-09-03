#include "common_headers/io.h"
#include "common_headers/types.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

static uint8_t get_rtc_register(int reg) 
{
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

static uint8_t bcd_to_bin(uint8_t val) 
{
    return (val & 0x0F) + ((val / 16) * 10);
}

void rtc_get_time(uint32_t* h, uint32_t* m, uint32_t* s) 
{
    while (get_rtc_register(0x0A) & 0x80);

    *s = bcd_to_bin(get_rtc_register(0x00));
    *m = bcd_to_bin(get_rtc_register(0x02));
    *h = bcd_to_bin(get_rtc_register(0x04));
}