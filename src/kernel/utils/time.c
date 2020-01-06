#ifndef TIME
#define TIME

void wait(uint32_t milliseconds) {
    if (milliseconds == 0) {
        return;
    }
    waiting = 1;
    check:
    asm volatile ("hlt");
    if (time_count < milliseconds) {
        goto check;
    }
    waiting = 0;
    time_count = 0;
};

#endif