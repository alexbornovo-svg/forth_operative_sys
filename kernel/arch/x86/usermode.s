global enter_usermode
global usermode_return

section .bss

saved_kernel_esp: resd 1

section .text

enter_usermode:
    mov eax, [esp + 4]
    mov edx, [esp + 8]
    push ebp
    push ebx
    push esi
    push edi
    mov [saved_kernel_esp], esp
    mov cx, 0x23
    mov ds, cx
    mov es, cx
    mov fs, cx
    mov gs, cx
    push 0x23
    push edx
    pushfd
    pop ecx
    or ecx, 0x200
    push ecx
    push 0x1B
    push eax
    iret

usermode_return:
    mov esp, [saved_kernel_esp]
    pop edi
    pop esi
    pop ebx
    pop ebp
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    sti
    ret

section .note.GNU-stack noalloc noexec nowrite progbits