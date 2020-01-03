#ifndef INT_FRAME
#define INT_FRAME
struct interrupt_frame;
#endif

#include "../../utils/text_mode.c"

__attribute__((interrupt)) void int32(struct interrupt_frame* frame) {
    print("\nclock\n");
    outb(0x20,0x20);
}