#ifndef INT_FRAME
#define INT_FRAME
struct interrupt_frame;
#endif

#ifndef KEYBOARD
#define KEYBOARD

#include <kernel/utils/io.c>

// keyboard maps and stuff are stolen from exscapeOS

/* A US keymap, courtesy of Bran's tutorial */
unsigned char kbdmix[128] = {
        0, 27, '1', '2', '3', '4', '5', '6', '7', '8',    /* 9 */
        '9', '0', '+', /*'´' */0, '\b',    /* Backspace */
        '\t',            /* Tab */
        'q', 'w', 'e', 'r',    /* 19 */
        't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',    /* Enter key */
        0,            /* 29   - Control */
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',    /* 39 */
        '\'', '<', 0,        /* Left shift */
        '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
        'm', ',', '.', '-', 0,                /* Right shift */
        '*',
        0,    /* Alt */
        ' ',    /* Space bar */
        0,    /* Caps lock */
        0,    /* 59 - F1 key ... > */
        0, 0, 0, 0, 0, 0, 0, 0,
        0,    /* < ... F10 */
        0,    /* 69 - Num lock*/
        0,    /* Scroll Lock */
        0,    /* Home key */
        0,    /* Up Arrow */
        0,    /* Page Up */
        '-',
        128,    /* Left Arrow */
        0,
        129,    /* Right Arrow */
        '+',
        0,    /* 79 - End key*/
        0,    /* Down Arrow */
        0,    /* Page Down */
        0,    /* Insert Key */
        0,    /* Delete Key */
        0, 0, '<',
        0,    /* F11 Key */
        0,    /* F12 Key */
        0,    /* All other keys are undefined */
};

// Excerpt from the US no-modifier key-table
//'q', 'w', 'e', 'r',	/* 19 */
//'t', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
//0,			/* 29   - Control */
//'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
//'\'', '`',   0,		/* Left shift */
//'<', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
//'m', ',', '.', '-',   0,				/* Right shift */

unsigned char kbdse_shift[128] =
        {
                0, 27, '!', '\"', '#', 0 /* shift+4 */, '%', '&', '/', '(',    /* 9 */
                ')', '=', '?', '`', '\b',    /* Backspace */
                '\t',            /* Tab */

                'Q', 'W', 'E', 'R',   /* 19 */
                'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'A', '\n', /* Enter key */
                0,          /* 29   - Control */
                'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'O', /* 39 */
                '\'', '>', 0,        /* Left shift */
                '*', 'Z', 'X', 'C', 'V', 'B', 'N',            /* 49 */
                'M', ';', ':', '_', 0,              /* Right shift */

                '*',
                0,    /* Alt */
                ' ',    /* Space bar */
                0,    /* Caps lock */
                0,    /* 59 - F1 key ... > */
                0, 0, 0, 0, 0, 0, 0, 0,
                0,    /* < ... F10 */
                0,    /* 69 - Num lock*/
                0,    /* Scroll Lock */
                0,    /* Home key */
                0,    /* Up Arrow */
                0,    /* Page Up */
                '-',
                0,    /* Left Arrow */
                0,
                0,    /* Right Arrow */
                '+',
                0,    /* 79 - End key*/
                0,    /* Down Arrow */
                0,    /* Page Down */
                0,    /* Insert Key */
                0,    /* Delete Key */
                0, 0, '>',
                0,    /* F11 Key */
                0,    /* F12 Key */
                0,    /* All other keys are undefined */
        };

unsigned char kbdse_alt[128] =
        {
                0, 27, 0 /*alt+1*/, '@', 0, '$', 0, 0, '{', '[',    /* 9 */
                ']', '}', '\\', '=', '\b',    /* Backspace */
                '\t',            /* Tab */
                'q', 'w', 'e', 'r',    /* 19 */
                't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',    /* Enter key */
                0,            /* 29   - Control */
                'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',    /* 39 */
                '\'', '`', 0,        /* Left shift */
                '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
                'm', ',', '.', '/', 0,                /* Right shift */
                '*',
                0,    /* Alt */
                ' ',    /* Space bar */
                0,    /* Caps lock */
                0,    /* 59 - F1 key ... > */
                0, 0, 0, 0, 0, 0, 0, 0,
                0,    /* < ... F10 */
                0,    /* 69 - Num lock*/
                0,    /* Scroll Lock */
                0,    /* Home key */
                0,    /* Up Arrow */
                0,    /* Page Up */
                '-',
                0,    /* Left Arrow */
                0,
                0,    /* Right Arrow */
                '+',
                0,    /* 79 - End key*/
                0,    /* Down Arrow */
                0,    /* Page Down */
                0,    /* Insert Key */
                0,    /* Delete Key */
                0, 0, '|',
                0,    /* F11 Key */
                0,    /* F12 Key */
                0,    /* All other keys are undefined */
        };


void null_handler(char key) {
    // do nothing
}


void (*key_down_handler)(char) = null_handler;

void (*key_up_handler)(char) = null_handler;

void set_key_down_handler(void (*handler)(char)) {
    key_down_handler = handler;
}

void set_key_up_handler(void (*handler)(char)) {
    key_up_handler = handler;
}


#define MOD_NONE  0
#define MOD_CTRL  (1 << 0)
#define MOD_SHIFT (1 << 1)
#define MOD_ALT   (1 << 2)

/* The modifier keys currently pressed */
static unsigned char mod_keys = 0;

__attribute__((interrupt)) void int33(struct interrupt_frame *frame) {
    uint8_t scancode = inb(0x60);
    switch (scancode) {
        case 0x2a: /* shift down */
        case 0x36: /* right shift down */
            mod_keys |= MOD_SHIFT;
            break;
        case 0xaa: /* shift up */
        case 0xb6: /* right shift up */
            mod_keys &= ~MOD_SHIFT;
            break;

        case 0x1d: /* ctrl down */
            mod_keys |= MOD_CTRL;
            break;
        case 0x9d: /* ctrl up */
            mod_keys &= ~MOD_CTRL;
            break;

        case 0x38: /* alt down */
            mod_keys |= MOD_ALT;
            break;
        case 0xb8: /* alt up */
            mod_keys &= ~MOD_ALT;
            break;
        default:
            break;
    }
    if ((scancode & 0x80) == 0x80) {
        if (mod_keys == MOD_NONE) {
            // No modifiers
            key_up_handler(kbdmix[scancode]);
        } else if (mod_keys == MOD_SHIFT) {
            // Shift + key
            key_up_handler(kbdse_shift[scancode]);
        } else if (mod_keys == MOD_ALT) {
            // Alt + key
            key_up_handler(kbdse_alt[scancode]);
        }
    } else {
        if (mod_keys == MOD_NONE) {
            // No modifiers
            key_down_handler(kbdmix[scancode]);
        } else if (mod_keys == MOD_SHIFT) {
            // Shift + key
            key_down_handler(kbdse_shift[scancode]);
        } else if (mod_keys == MOD_ALT) {
            // Alt + key
            key_down_handler(kbdse_alt[scancode]);
        }
    }
    outb(0x20, 0x20);
}

#endif