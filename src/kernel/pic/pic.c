#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_DATA 0x21

#define PIC_SLAVE_CMD 0xA0
#define PIC_SLAVE_DATA 0xA1

#define ICW1_ICW4	0x01    // flag for init sequence to skip stage 4 of initiation.
#define ICW1_INIT	0x10    // first 4 bits for sequence ID, here the sequence will be 1.

#define ICW4_8086	0x01    //8086/88 (MCS-80/85) mode

#define io_wait() ({ outb(0x80, 0x00); })

#include "../utils/io.c"

void remap_pic() {
    uint8_t master_mask = inb(PIC_MASTER_DATA); // save mask for master PIC
    uint8_t slave_mask = inb(PIC_SLAVE_DATA); // save mask for slave PIC

    /*
     * This starts the initialization sequence of remapping the PIC. Here, we signal to the master PIC that we would
     * like to start the initialization sequence by setting the sequence ID bits (first 4 bits) to 1. We also set the
     * flags of the sequence to 1, to signal that we would like to skip stage 4.
     */
    outb(PIC_MASTER_CMD,  ICW1_INIT | ICW1_ICW4); // 0b0001 0b0001
    io_wait();
    outb(PIC_MASTER_DATA, 32); // set the master's IRQs to map to the IDT entries after 32
    io_wait();
    outb(PIC_MASTER_DATA, 4); // alert master that slave is connected via IRQ 2
    io_wait();
    outb(PIC_MASTER_DATA, ICW4_8086); // tell the PIC to work in x86 mode

    /*
     * Redo the initialization sequence for the slave PIC. For this, we set the offset of the IDT entries to 40, which
     * is directly after the IDT entries for the master PIC
     */
    outb(PIC_SLAVE_CMD,  ICW1_INIT | ICW1_ICW4); // 0b0001 0b0001
    io_wait();
    outb(PIC_SLAVE_DATA, 40); // set the master's IRQs to map to the IDT entries after 32
    io_wait();
    outb(PIC_MASTER_DATA, 2); // alert slave of cascade ID
    io_wait();
    outb(PIC_SLAVE_DATA, ICW4_8086); // tell the PIC to work in x86 mode
    io_wait();

    outb(PIC_MASTER_DATA, 0xfc); // enable clock and keyboard
    outb(PIC_SLAVE_DATA, 0xff);
}