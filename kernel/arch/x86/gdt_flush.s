global gdt_flush

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    mov ax, 0x10        ; 0x10 kernel data offset
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.reload_cs ; far jump

.reload_cs:
    ret