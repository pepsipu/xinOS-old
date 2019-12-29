all: setup bootloader kernel image

BUILD_DIR=build
OS_IMG=shrineos.img

setup:
	mkdir -p ./$(BUILD_DIR)
	mkdir -p ./$(BUILD_DIR)/bootloader
	mkdir -p ./$(BUILD_DIR)/kernel

bootloader:
	# compile bootloader as raw machine code
	nasm -f bin src/boot/bootloader.asm -o $(BUILD_DIR)/bootloader/bootloader.bin

kernel:
	# compile an object file of the kernel assuming a standard library won't be available
	gcc -ffreestanding -c src/kernel/kernel.c -o $(BUILD_DIR)/kernel/kernel.o
	# link the kernel relative to 0x7f00 (we will load it at this address) and the format as a raw binary file
	ld -o $(BUILD_DIR)/kernel/kernel.bin -Ttext 0x7f00 --oformat binary $(BUILD_DIR)/kernel/kernel.o

image:
	# create a blank image 256kb large
	dd if=/dev/zero of=$(BUILD_DIR)/shrineos.img bs=512 count=64
	# write bootloader and it's strings/data to first 1024 bytes
	dd conv=notrunc if=$(BUILD_DIR)/bootloader/bootloader.bin of=$(BUILD_DIR)/$(OS_IMG) bs=512 count=2 seek=0
	# write kernel to the rest of image
	dd conv=notrunc if=$(BUILD_DIR)/kernel/kernel.bin of=$(BUILD_DIR)/$(OS_IMG) bs=512 count=10 seek=2

clean:
	rm -r build