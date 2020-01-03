gdb -ex "set arch i386:intel" -ex "symbol-file build/kernel/kernel.elf" -ex "gef-remote -q localhost:1234" -ex "break *0x500" -ex "c"
#gdb -ex "set arch i386:intel" -ex "symbol-file build/bootloader/bootloader.elf" -ex "gef-remote -q localhost:1234" -ex "break *0x7c00" -ex "c"
