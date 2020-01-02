#ifndef INT_FRAME
#define INT_FRAME
struct interrupt_frame;
#endif

#include "../../utils/text_mode.c"

__attribute__((interrupt)) void int08(struct interrupt_frame* frame) {
    print("\ndouble fault, nice job sam, you're clearly an amazing programmer\n");
    print("clearly going to contribute to the linux kernel anytime now \n");
    asm("hlt");
}