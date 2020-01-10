#include <kernel/idt/idt.c>
#include <kernel/pic/pic.c>
#include <kernel/vesa/scenes/main_menu.c>

/*
 * A lot of CPU related sizes have 1 subtracted from their actual size. Why? Fantastic question. After asking Michael
 * Petch, he said "BIOS designers wanted people scratching their heads 35 years later", which makes sense. I'll be
 * calling this "rule 1" so I don't need to explain why I keep subtracting one from what appears to be a regular size.
 * (https://i.gyazo.com/83777d7d9efa55725eb9bfc5ba0f11c6.png)
 * (in reality, it's for saving costs on comparator hardware, but I won't get into that)
 */

void __attribute__ ((section ("kernel_entry"))) kmain() {
    //play_wav(menu_navigate_wav);
    // init_screen();
    // print("Initialized 32bit protected mode display.\n");
    remap_pic();

    init_idt();
    // print("Initialized the interrupt descriptor table.\n");
    load_main_menu();
    // print("Remapped the PIC.\n");

}
