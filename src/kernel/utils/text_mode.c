// this code is not used. this is for text mode, but xinOS will not use text mode, so this code is kept for possible error catching in the future.

/*#ifndef DISPLAY
#define DISPLAY

#include "io.c"
#include "mem.c"
#include "strings.c"

uint16_t max_width = 0;
uint8_t max_height = 0;

struct {
    uint16_t x;
    uint8_t y;
} pos = {
        .x = 0,
        .y = 0
};

void scroll() {
    for (int i = 1; i < max_height; i++) {
        memcpy(0xb8000 + (max_width * ((i - 1) * 2)), 0xb8000 + (max_width * (i * 2)), max_width * 2);
    }
}

void inc_y() {

}

void print(char *s) {
    int len = string_len(s);
    for (int i = 0; i < len; i++) {
        if (pos.x + 1 > max_width) {
            if (pos.y + 1 > max_height) {
                scroll();
            } else {
                pos.y++;
                pos.x = 0;
            }

        }
        if (s[i] == '\n') {
            if (pos.y + 1 > max_height) {
                scroll();
            } else {
                pos.y++;
                pos.x = 0;
            }
            continue;
        }
        *((char *) 0xb8000 + (max_width * (pos.y * 2)) + (pos.x * 2)) = s[i]; // multiply by 2 since each cell in display is a word, one for ascii and one for colors
        *((char *) 0xb8000 + (max_width * (pos.y * 2)) + ((pos.x * 2) + 1)) = 0xf;
        pos.x++;
    }
}

void clear_screen() {
    memset((void *) 0xb8000, 0, (max_width * max_height) * 2);
    pos.y = 0;
    pos.x = 0;
}

void init_screen() {
    max_width = *((uint16_t *) 0x044a); // read column count from bios data
    max_height = *((uint8_t *) 0x0484) + 1; // read row count from bios data, and add one because of rule 1
    clear_screen();
}

#endif
*/