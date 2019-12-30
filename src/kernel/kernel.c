#include "allocator.c"
#include "idt/idt.c"
#include "definitions.h"

int __attribute__ ((section ("kernel_entry"))) kmain() {
    init_idt();
}