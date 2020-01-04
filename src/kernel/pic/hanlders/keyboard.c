#ifndef INT_FRAME
#define INT_FRAME
struct interrupt_frame;
#endif

#ifndef KEYBOARD
#define KEYBOARD

#include "../../vesa/vesa.c"

static const uint8_t ascii_nomod[] = {
        '\0', '\e', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\0', 'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', '\0', '\0', '\0', ' '
};

void null_handler(char key) {
    // do nothing
}



void (*key_down_handler)(char) = null_handler;
void (*key_up_handler)(char) = null_handler;

__attribute__((interrupt)) void int33(struct interrupt_frame* frame) {
    uint8_t scancode = inb(0x60);
    if ((scancode & 128) == 128) {
        key_up_handler(ascii_nomod[scancode & 127]);
    } else {
        key_down_handler(ascii_nomod[scancode]);
    }
    outb(0x20,0x20);
}

#endif