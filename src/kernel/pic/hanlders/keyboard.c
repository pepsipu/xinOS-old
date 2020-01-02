#ifndef INT_FRAME
#define INT_FRAME
struct interrupt_frame;
#endif
static const uint8_t ascii_nomod[] = {
        '\0', '\e', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0', 'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', '\0', '\0', '\0', ' '
};
#include "../../utils/text_mode.c"

__attribute__((interrupt)) void int33(struct interrupt_frame* frame) {
    char key[2];
    uint8_t scancode = inb(0x60);
    if ((scancode & 128) == 128) {
        //print("released\n");
    } else {
        //print("pressed\n");
        key[0] = ascii_nomod[scancode];
        key[1] = 0;
        print(key);
    }
    outb(0x20,0x20);
}