#include "allocator.c"

int __attribute__ ((section ("kernel_entry"))) kmain() {
    *((volatile char *) 0xb8000) = '6';
    *((volatile char *) 0xb8002) = '9';
    long int *buffer = alloc(0x10);
    *buffer = 0xdeadbeef;
    long int *buffer2 = alloc(0x20);
    *buffer2 = 0xc0ded00d;
    free(buffer);
    long int *new = alloc(0x4);
}