global user_forth_start
global user_forth_end

section .data

user_forth_start:
    incbin "user/forth.bin"
user_forth_end:

section .note.GNU-stack noalloc noexec nowrite progbits