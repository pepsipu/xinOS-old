make all && qemu-system-i386 -drive file=./build/xinos.img,format=raw,index=0,media=disk -s -machine q35 -soundhw pcspk -accel kvm
