BUILD=build
CFLAGS=-I . -std=gnu99 -ffreestanding -Wall -Wextra -m32 -fno-pic -no-pie -g -mgeneral-regs-only -mno-red-zone -fno-stack-protector -Os
CC=gcc
AS=nasm
LD=gcc
LDFLAGS=-T link.ld -ffreestanding -O2 -nostdlib -lgcc -m32

CSOURCES=$(shell find . -type f -iname '*.c')
COBJECTS=$(foreach x, $(basename $(CSOURCES)), $(x).o)
ASMSOURCES=$(shell find . -type f -iname '*.asm')
ASMOBJECTS=$(foreach x, $(basename $(ASMSOURCES)), $(x).asm.o)
BIN=xinOS.bin
ISONAME=xinOS.iso

OBJS=$(COBJECTS) $(ASMOBJECTS)

.PHONY: clean mkdirs $(BIN) iso

all: mkdirs $(BIN) iso

mkdirs:
	echo $(ASMOBJECTS)
	echo $(ASMSOURCES)
	mkdir -p $(BUILD)
	mkdir -p $(BUILD)/iso/boot/grub

iso:
	cp grub.cfg $(BUILD)/iso/boot/grub
	cp $(BIN) $(BUILD)/iso/boot
	grub-mkrescue -o $(BUILD)/$(ISONAME) $(BUILD)/iso

$(BIN): $(COBJECTS) $(ASMOBJECTS) 
	$(LD) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm $(OBJS)
	rm -r $(BUILD)
	rm $(BIN)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.asm.o: %.asm
	$(AS) $< -I . -f elf32 -o $@