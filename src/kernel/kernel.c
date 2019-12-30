#include "allocator.c"

char test = 'A';

int __attribute__ ((section ("kernel_entry"))) kmain() {
    *((volatile char *) 0xb8000) = test;
}