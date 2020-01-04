#ifndef TIME
#define TIME

void wait(int time) {
    waiting = 1;
    uint32_t wait_until = time_count + time;
    check:
    asm volatile ("hlt");
    if (time_count < wait_until) {
        goto check;
    }
    waiting = 0;
    time_count = 0;
};

#endif