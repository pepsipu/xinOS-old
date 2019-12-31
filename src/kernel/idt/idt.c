#include <stdint.h>
#include "../utils/mem.c"
#include "../definitions.h"


/*
 * This code sets up the Interrupt Descriptor table (idt). This is used for handling software and hardware interrupts.
 * More information can be found at https://wiki.osdev.org/IDT.
*/
struct idt_entry {
    uint16_t offset_1; // offset bits 0..15
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t zero;      // unused, set to 0
    uint8_t type; // type and attributes, see below
    uint16_t offset_2; // offset bits 16..31
};

struct idt_ptr {
    uint16_t size;
    uint32_t addr;
};

struct idt_entry idt[256];
struct idt_ptr idtp;

/*
 * An interrupt service routine (isr) is a function to run when a service is called. A handy helper function will take
 * in a entry in the IDT (0 - 255) and correspond a function to that entry. the idt variable holds all 256 entries.
 */
void register_isr(void *isr, uint8_t index) { // register a interrupt service routine
    struct idt_entry *current_entry = &idt[index];
    /*
     * Due to some weird fragmentation issues, our pointer must be split in half. This is done by storing the pointer as
     * a character array, where each element of the array is one byte of the address. For example, getting the low
     * bytes of 0xDEADBEEF (little endianness matters), you could cast the pointer as 4 characters and get the first
     * two "characters". This would return EF and BE. ([EF BE] AD DE)
     */
    char *isr_byte_array = isr;
    current_entry->offset_1 = (uint16_t) *isr_byte_array; // get low bytes
    current_entry->offset_2 = (uint16_t) *(isr_byte_array + 2); // get high bytes
    current_entry->zero = 0; // must be set to 0, for whatever reason
    /*
     * Flags are a big messy to work with in a high level language. As a visual aid, follow the diagram (provided by
     * the OSDev wiki.)
     *   7                           0
     * +---+---+---+---+---+---+---+---+
     * | P |  DPL  | S |    GateType   |
     * +---+---+---+---+---+---+---+---+
     * Present (P) = 1, because it is used (this bit is used to tell if the interrupt is being handled)
     * Descriptor Privilege Level (DPL) = 00 (ring 0), because all interrupts will run in kernel mode
     * Storage Segment (S) = 0, as this is either a trap or interrupt
     * Gate Type = 1110, the gate for 32 bit interrupts
     * 0b10001110 = 0x8E
     */
    current_entry->type = 0x8E;
}

/*
 * The init_idt function will set up the idtp (interrupt descriptor table pointer), a pointer to a structure that
 * describes the idt, which is not to be confused with the idt itself. This structure will store both the size and
 * address of the idt.
 */

void init_idt() {
    idtp.size = (sizeof (struct idt_entry) * 256) - 1; // For whatever reason, the CPU will take in the size - 1.
    idtp.addr = (uint32_t) &idt;
    memset(&idt, 0, 256 * sizeof(struct idt_entry)); // Zero out the memory of the IDT (just in case) to avoid old values from acting as entries
    asm("lidt %0" : : "m"(idtp)); // load the idt, equivalent to "lidt [idtp]"
}