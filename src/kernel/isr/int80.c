#ifndef INT_FRAME
#define INT_FRAME
struct interrupt_frame;
#endif

#include "../utils/display.c"

__attribute__((interrupt)) void int80(struct interrupt_frame* frame) {
    print("unlucky dude");
}