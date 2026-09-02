#include "ps2kbd.h"
#include "common_headers/io.h"
#include "common_headers/types.h"

uchar kbd_wait()
{
    return inb(0x64) & 0x01;
}


