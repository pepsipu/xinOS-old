#include <stdint.h>
#include "../utils/mem.c"
#include "../definitions.h"

struct idt_entry {
    uint16_t offset_1; // offset bits 0..15
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t zero;      // unused, set to 0
    uint8_t type_attr; // type and attributes, see below
    uint16_t offset_2; // offset bits 16..31
};

struct idt_ptr {
    uint16_t size;
    uint32_t addr;
};

struct idt_entry idt[256];
struct idt_ptr idtp;


void register_isr(void *isr, uint8_t index) { // register a interrupt service routine

}

void init_idt() {
    idtp.size = (sizeof (struct idt_entry) * 256) - 1;
    idtp.addr = (uint32_t) &idt;
    memset(&idt, 0, 256 * sizeof(struct idt_entry));
    asm("lidt %0" : : "m"(idtp)); // load the idt
}