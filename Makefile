all:setup bootloader image

BUILD_DIR=build
OS_IMG=shrineos.img

setup:
	mkdir -p ./$(BUILD_DIR)
	mkdir -p ./$(BUILD_DIR)/bootloader

bootloader:
	# compile bootloader as raw machine code
	nasm -f bin bootloader.asm -o ./$(BUILD_DIR)/bootloader/bootloader.bin

image:
	# create a blank image 256kb large
	dd if=/dev/zero of=./$(BUILD_DIR)/shrineos.img bs=512 count=64
	# write bootloader and it's strings/data to first 1024 bytes
	dd conv=notrunc if=./$(BUILD_DIR)/bootloader/bootloader.bin of=./$(BUILD_DIR)/$(OS_IMG) bs=512 count=2 seek=0

clean:
	rm -r build