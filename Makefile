# Toolchain
AS = nasm
CC = gcc
LD = ld

# Flag di compilazione
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -Ikernel
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

# File sorgente e oggetti
ASM_SRC = boot/boot.s
C_SRC = $(shell find kernel -type f -name '*.c')

ASM_OBJ = boot/boot.o
C_OBJ = $(C_SRC:.c=.o)
OBJS = $(ASM_OBJ) $(C_OBJ)

# Output
KERNEL_BIN = iso/boot/mykernel.bin
ISO_OUT = forth_os.iso

.PHONY: all clean run iso

# Target predefinito: crea l'immagine ISO
all: $(ISO_OUT)

# Regola per l'assembler
boot/boot.o: $(ASM_SRC)
	$(AS) $(ASFLAGS) $< -o $@

# Regola per i file C nel kernel
kernel/%.o: kernel/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Linking dell'eseguibile ELF del kernel
$(KERNEL_BIN): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# Creazione dell'immagine ISO avviabile tramite GRUB
$(ISO_OUT): $(KERNEL_BIN)
	grub-mkrescue -o $(ISO_OUT) iso

# Avvio rapido su QEMU
run: $(ISO_OUT)
	qemu-system-i386 -cdrom $(ISO_OUT)

# Pulizia dei file generati
clean:
	rm -f $(OBJS) $(KERNEL_BIN) $(ISO_OUT)