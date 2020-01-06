#include "idt/idt.c"
#include "isr/int80.c"
#include "pic/pic.c"
#include "vesa/scenes/main_menu.c"
#include "sound/pc_speaker.c"
#include "sound/wav_parser.c"
#include "../music/test_audio.h"

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

    int divisor = 1193180 / 1193; // divide PIT's MHz by target hertz
    outb(0x43, 0x36); // command byte to set channel 0 (timer)
    outb(0x40, divisor & 0xFF); // send low
    outb(0x40, divisor >> 8); // send high

    for (int i = 0; i < sizeof(mario) / sizeof(struct beep); i++) {
        beep(mario[i].freq, mario[i].len, mario[i].delay);
    }

    // print("Remapped the PIC.\n");
}
