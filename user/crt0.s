global _start
extern main

section .text.start

_start:
    call main
    mov ebx, eax
    mov eax, 0
    int 0x80
.hang:
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite progbits