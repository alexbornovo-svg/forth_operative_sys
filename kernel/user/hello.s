global user_hello_start
global user_hello_end

section .data

user_hello_start:
    call .next
.next:
    pop esi
    lea ebx, [esi + (prompt - .next)]
    mov ecx, prompt_len
    mov eax, 1
    int 0x80
    sub esp, 64
    mov ebx, esp
    mov ecx, 64
    mov eax, 2
    int 0x80
    mov ecx, eax
    mov ebx, esp
    mov eax, 1
    int 0x80
    mov eax, 0
    xor ebx, ebx
    int 0x80
.hang:
    jmp .hang

prompt: db "Write something: "
prompt_len equ $ - prompt

user_hello_end:

section .note.GNU-stack noalloc noexec nowrite progbits