#ifndef INT_FRAME
#define INT_FRAME
struct interrupt_frame;
#endif

__attribute__((interrupt)) void int01(struct interrupt_frame* frame) {

}