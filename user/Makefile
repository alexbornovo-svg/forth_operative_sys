CC = gcc
AS = nasm
LD = ld
OBJCOPY = objcopy

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -fno-tree-loop-distribute-patterns -fwrapv -I.
LDFLAGS = -m elf_i386 -T user.ld -nostdlib -z noexecstack -z max-page-size=0x1000

USER_BIN = user/forth.bin

.PHONY: all clean

all: forth.bin

$(USER_BIN): $(wildcard user/*.c user/*.h user/*.s user/user.ld user/Makefile)
	$(MAKE) -C user

kernel/user/forth_image.o: $(USER_BIN)

crt0.o: crt0.s
	$(AS) -f elf32 crt0.s -o crt0.o

%.o: %.c ulib.h
	$(CC) $(CFLAGS) -c $< -o $@

forth.elf: crt0.o ulib.o forth.o user.ld
	$(LD) $(LDFLAGS) -o $@ crt0.o ulib.o forth.o

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f *.o *.elf *.bin

