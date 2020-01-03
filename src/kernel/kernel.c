#include "allocator.c"
#include "idt/idt.c"
#include "definitions.h"
#include "isr/int80.c"
#include "utils/text_mode.c"
#include "pic/pic.c"
#include "vesa/vesa.c"

/*
 * A lot of CPU related sizes have 1 subtracted from their actual size. Why? Fantastic question. After asking Michael
 * Petch, he said "BIOS designers wanted people scratching their heads 35 years later", which makes sense. I'll be
 * calling this "rule 1" so I don't need to explain why I keep subtracting one from what appears to be a regular size.
 * (https://i.gyazo.com/83777d7d9efa55725eb9bfc5ba0f11c6.png)
 */

void __attribute__ ((section ("kernel_entry"))) kmain() {
    test();
    init_screen();
    // print("Initialized 32bit protected mode display.\n");
    remap_pic();
    // print("Remapped the PIC.\n");
    init_idt();
    // print("Initialized the interrupt descriptor table.\n");
}
