#ifndef INT_FRAME
#define INT_FRAME
struct interrupt_frame;
#endif

__attribute__((interrupt)) void int32(struct interrupt_frame* frame) {
    if (waiting) {
        time_count++;
    }
    outb(0x20,0x20);
}