#include "pit.h"
#include "pic.h"
#include "arch/x86/isr.h"
#include "common_headers/io.h"
#include "rtc.h"

#define PIT_CHANNEL_0 0x40
#define PIT_COMMAND   0x43
#define PIT_BASE_FREQ 1193182

static volatile uint32_t timer_ticks = 0;
static uint32_t seconds = 0;
static uint32_t minutes = 0;
static uint32_t hours   = 0;

static void draw_clock(void) 
{
    volatile uint16_t* vga_buffer = (volatile uint16_t*)0xB8000;
    
    int offset = 0 * 80 + 70;
    
    uint16_t color = 0x0F00;

    char clock_str[9];
    clock_str[0] = '0' + (hours / 10);
    clock_str[1] = '0' + ((hours + 2) % 10);
    clock_str[2] = ':';
    clock_str[3] = '0' + (minutes / 10);
    clock_str[4] = '0' + (minutes % 10);
    clock_str[5] = ':';
    clock_str[6] = '0' + (seconds / 10);
    clock_str[7] = '0' + (seconds % 10);
    clock_str[8] = '\0';

    for (int i = 0; i < 8; i++) 
    {
        vga_buffer[offset + i] = color | clock_str[i];
    }
}

static void pit_callback(registers_t* regs) 
{
    (void)regs;
    timer_ticks++;

    if (timer_ticks % 100 == 0) 
    {
        seconds++;
        if (seconds >= 60) 
        {
            seconds = 0;
            minutes++;
            if (minutes >= 60) 
            {
                minutes = 0;
                hours = (hours + 1) % 24;
            }
        }
        draw_clock();
    }

    pic_send_eoi(0);
}

void pit_init(uint32_t frequency) 
{
    // reading the hour through the rtc
    rtc_get_time(&hours, &minutes, &seconds);

    register_interrupt_handler(32, pit_callback);

    uint32_t divisor = PIT_BASE_FREQ / frequency;
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL_0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL_0, (uint8_t)((divisor >> 8) & 0xFF));

    draw_clock();
}
uint32_t pit_get_ticks(void) 
{
    return timer_ticks;
}