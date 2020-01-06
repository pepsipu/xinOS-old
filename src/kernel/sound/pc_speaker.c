#include "../utils/time.c"

void set_pit(uint32_t div) {
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t) (div));
    outb(0x42, (uint8_t) (div >> 8));

}

void play_sound(uint32_t freq) {
    uint32_t div;
    uint8_t tmp;
    div = 1193182 / freq; // frequency of PIT runs at 1.193182MHz, so dividing it by our Hz frequency will give us the MHz equivalent
    //Set the PIT to the desired frequency
    set_pit(div);

    //And play the sound using the PC speaker
    tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
        outb(0x61, tmp | 3);
    }
}

//make it shutup
void nosound() {
    outb(0x61, inb(0x61) & 0xFC);
}

void beep(uint16_t freq, uint16_t len, uint16_t time_in_between) {
    play_sound(freq);
    wait(len);
    nosound();
    wait(time_in_between);
}

