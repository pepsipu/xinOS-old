#ifndef INT_FRAME
#define INT_FRAME
struct interrupt_frame {
    uint32_t err_code;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
};
#endif

#include <kernel/vesa/graphics.c>
#include <kernel/utils/strings.c>
#include <kernel/allocator.c>


#define calc_line(x) x * 16
#define ASSIGN_HEX " = 0x"


char buff[10];

char *exception_messages[32] = {
        "Division by zero",
        "Debug",
        "Non-maskable interrupt",
        "Breakpoint",
        "Detected overflow",
        "Out-of-bounds",
        "Invalid opcode",
        "No coprocessor",
        "Double fault",
        "Coprocessor segment overrun",
        "Bad TSS",
        "Segment not present",
        "Stack fault",
        "General protection fault",
        "Page fault",
        "Unknown interrupt",
        "Coprocessor fault",
        "Alignment check",
        "Machine check",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved"
};

uint8_t line = 6;

void draw_error(char *str, int val) {
    int len = string_len(str) * 8;
    draw_string(str, 0, calc_line(line), 0xffff);
    draw_string(ASSIGN_HEX, len, calc_line(line), 0xffff);
    itoa(val, buff, 16);
    draw_string(buff, len + 5 * 8, calc_line(line), 0xffff);
    line++;
}

__attribute__((interrupt)) void int08(struct interrupt_frame* frame) {
    draw_background(0);
    draw_string("Oh noes! xinOS crashed :(", 0, 0, 0xffff);
    draw_string("Please report this on github. It really helps patch bugs.", 0, calc_line(1), 0xffff);
    draw_string("Though, this error might be the fault of the game you're running.", 0, calc_line(2), 0xffff);
    draw_string("DEVELOPER INFO:", 0, calc_line(4), 0xffff);

    draw_error("ERROR_CODE", frame->err_code);
    draw_string(exception_messages[frame->err_code], 0, calc_line(line), 0xffff);
    line += 2;
    draw_error("FLAGS", frame->flags);
    draw_error("CS", frame->cs);
    draw_error("IP", frame->ip);


    asm("cli");
    asm("hlt");
}