# Toolchain
AS = nasm
CC = gcc
LD = ld

# Flag di compilazione
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -Ikernel
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

# Ricerca automatica dei file sorgente C e Assembly (.s o .asm)
C_SRC   = $(shell find kernel -type f -name '*.c')
ASM_SRC = $(shell find boot kernel -type f \( -name '*.s' -o -name '*.asm' \))

# Conversione dei file sorgente nei corrispettivi oggetti (.o)
C_OBJ   = $(C_SRC:.c=.o)
ASM_OBJ = $(patsubst %.s,%.o,$(patsubst %.asm,%.o,$(ASM_SRC)))

OBJS = $(ASM_OBJ) $(C_OBJ)

# Output
KERNEL_BIN = iso/boot/mykernel.bin
ISO_OUT = forth_os.iso

.PHONY: all clean run iso

# Target predefinito: crea l'immagine ISO
all: $(ISO_OUT)

# Regola generica per compilare qualsiasi file Assembly (.s)
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Regola generica per compilare qualsiasi file Assembly (.asm)
%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

# Regola generica per compilare qualsiasi file C
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Linking del kernel
$(KERNEL_BIN): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# Creazione dell'immagine ISO con GRUB
$(ISO_OUT): $(KERNEL_BIN)
	grub-mkrescue -o $(ISO_OUT) iso

# Avvio su QEMU
run: $(ISO_OUT)
	qemu-system-i386 -cdrom $(ISO_OUT)

# Pulizia dei file generati
clean:
	rm -f $(OBJS) $(KERNEL_BIN) $(ISO_OUT)