#include "speaker.h"
#include "pit.h"
#include "common_headers/io.h"

#define PIT_BASE_FREQUENCY 1193182
#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL2_PORT 0x42
#define SPEAKER_CONTROL_PORT 0x61
#define SPEAKER_MIN_FREQUENCY 20
#define SPEAKER_MAX_FREQUENCY 20000

void speaker_stop(void)
{
    uint8_t control = inb(SPEAKER_CONTROL_PORT);
    outb(SPEAKER_CONTROL_PORT, control & 0xFC);
}

void speaker_play(uint32_t frequency)
{
    if (frequency < SPEAKER_MIN_FREQUENCY)
    {
        speaker_stop();
        return;
    }

    if (frequency > SPEAKER_MAX_FREQUENCY)
    {
        frequency = SPEAKER_MAX_FREQUENCY;
    }

    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;

    outb(PIT_COMMAND_PORT, 0xB6);
    outb(PIT_CHANNEL2_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL2_PORT, (uint8_t)((divisor >> 8) & 0xFF));

    uint8_t control = inb(SPEAKER_CONTROL_PORT);

    if ((control & 0x03) != 0x03)
    {
        outb(SPEAKER_CONTROL_PORT, control | 0x03);
    }
}

void speaker_beep(uint32_t frequency, uint32_t duration_ms)
{
    speaker_play(frequency);
    pit_sleep(duration_ms);
    speaker_stop();
}