ENTRY(_start)

SECTIONS
{
    . = 0x00800000;

    .text :
    {
        *(.text.start)
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(.bss*)
        *(COMMON)
    }
}