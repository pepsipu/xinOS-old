#include <kernel/vesa/graphics.c>
#include <music/music_list.h>

#define XA_BIG_MAIN_COLOR 0x363333 // 0x2f3032
#define XA_BIG_SECOND_COLOR 0x272121 // 0x383a56
#define XA_BIG_HIGHLIGHT_2 0xe16428 // 0xb0a565
#define XA_BIG_HIGHLIGHT_1 0xf6e9e9 // 0xede68a

// shift the RBG888 colors into RBG565, with a small loss of detail
#define XA_MAIN_COLOR (((XA_BIG_MAIN_COLOR&0xf80000)>>8) + ((XA_BIG_MAIN_COLOR&0xfc00)>>5) + ((XA_BIG_MAIN_COLOR&0xf8)>>3))
#define XA_SECOND_COLOR (((XA_BIG_SECOND_COLOR&0xf80000)>>8) + ((XA_BIG_SECOND_COLOR&0xfc00)>>5) + ((XA_BIG_SECOND_COLOR&0xf8)>>3))
#define XA_HIGHLIGHT_2 (((XA_BIG_HIGHLIGHT_2&0xf80000)>>8) + ((XA_BIG_HIGHLIGHT_2&0xfc00)>>5) + ((XA_BIG_HIGHLIGHT_2&0xf8)>>3))
#define XA_HIGHLIGHT_1 (((XA_BIG_HIGHLIGHT_1&0xf80000)>>8) + ((XA_BIG_HIGHLIGHT_1&0xfc00)>>5) + ((XA_BIG_HIGHLIGHT_1&0xf8)>>3))

#define TITLE "XinAdventures"
#define SUBTITLE "A game to demonstrate the abilities of xinOS"

int xa_main() {
    draw_background(XA_MAIN_COLOR);
    draw_string(TITLE, center_x(string_len(TITLE) * 8, vbe_info->width), 50, XA_HIGHLIGHT_2);
    draw_string(SUBTITLE, center_x(string_len(SUBTITLE) * 8, vbe_info->width), 70, XA_HIGHLIGHT_1);
    draw_line(70, 0, 70, vbe_info->height, XA_SECOND_COLOR, 20);
    draw_line(vbe_info->width - 70 - 20, 0, vbe_info->width - 70 - 20, vbe_info->height, XA_SECOND_COLOR, 20);
    play_song(0);
    return 1;
}