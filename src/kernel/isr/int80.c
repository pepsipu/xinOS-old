#ifndef INT_FRAME
#define INT_FRAME
struct interrupt_frame;
#endif

__attribute__((interrupt)) void int80(struct interrupt_frame* frame) {
    *((char *) 0xb8000) = 'A';
}