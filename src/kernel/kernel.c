#include "allocator.c"
#include "idt/idt.c"
#include "definitions.h"
#include "isr/int80.c"

int __attribute__ ((section ("kernel_entry"))) kmain() {
    init_idt();
    register_isr(int80, 0x80);
    asm("int $0x80");
}