all: setup bootloader kernel image

BUILD_DIR=build
OS_IMG=shrineos.img

setup:
	mkdir -p ./$(BUILD_DIR)
	mkdir -p ./$(BUILD_DIR)/bootloader
	mkdir -p ./$(BUILD_DIR)/kernel

bootloader:
	# compile bootloader as raw machine code
	nasm -f elf32 -g3 -F dwarf src/boot/bootloader.asm -o $(BUILD_DIR)/bootloader/bootloader.o
	ld -Ttext=0x7c00 -melf_i386 $(BUILD_DIR)/bootloader/bootloader.o -o $(BUILD_DIR)/bootloader/bootloader.elf
	objcopy -O binary $(BUILD_DIR)/bootloader/bootloader.elf $(BUILD_DIR)/bootloader/bootloader.bin
kernel:
	# compile an object file of the kernel assuming a standard library won't be available
	gcc -ffreestanding -c src/kernel/kernel.c -o $(BUILD_DIR)/kernel/kernel.o -m32 -fno-pic -no-pie -g -mgeneral-regs-only -mno-red-zone
	# link the kernel relative to 0x7f00 (we will load it at this address) as an elf, for debugging
	ld -o $(BUILD_DIR)/kernel/kernel.elf -T src/kernel/kernel.lds $(BUILD_DIR)/kernel/kernel.o -melf_i386
	# create raw binary file of kernel
	objcopy -O binary $(BUILD_DIR)/kernel/kernel.elf $(BUILD_DIR)/kernel/kernel.bin

image:
	# create a blank image 32kb large
	dd if=/dev/zero of=$(BUILD_DIR)/shrineos.img bs=512 count=64
	# write bootloader and it's strings/data to first 1024 bytes (1kb)
	dd conv=notrunc if=$(BUILD_DIR)/bootloader/bootloader.bin of=$(BUILD_DIR)/$(OS_IMG) bs=512 count=2 seek=0
	# write kernel to the rest of image (2.5kb)
	dd conv=notrunc if=$(BUILD_DIR)/kernel/kernel.bin of=$(BUILD_DIR)/$(OS_IMG) bs=512 count=5 seek=2

clean:
	rm -r build