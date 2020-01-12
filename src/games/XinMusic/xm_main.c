#include <kernel/vesa/graphics.c>
#include <music/music_list.h>
#include <kernel/utils/misc.c>
#include <kernel/utils/rand.c>

#define XM_BIG_MAIN_COLOR 0x363333 // 0x2f3032
#define XM_BIG_SECOND_COLOR 0x272121 // 0x383a56
#define XM_BIG_HIGHLIGHT_2 0xe16428 // 0xb0a565
#define XM_BIG_HIGHLIGHT_1 0xf6e9e9 // 0xede68a

// shift the RBG888 colors into RBG565, with a small loss of detail
#define XM_MAIN_COLOR (((XM_BIG_MAIN_COLOR&0xf80000)>>8) + ((XM_BIG_MAIN_COLOR&0xfc00)>>5) + ((XM_BIG_MAIN_COLOR&0xf8)>>3))
#define XM_SECOND_COLOR (((XM_BIG_SECOND_COLOR&0xf80000)>>8) + ((XM_BIG_SECOND_COLOR&0xfc00)>>5) + ((XM_BIG_SECOND_COLOR&0xf8)>>3))
#define XM_HIGHLIGHT_2 (((XM_BIG_HIGHLIGHT_2&0xf80000)>>8) + ((XM_BIG_HIGHLIGHT_2&0xfc00)>>5) + ((XM_BIG_HIGHLIGHT_2&0xf8)>>3))
#define XM_HIGHLIGHT_1 (((XM_BIG_HIGHLIGHT_1&0xf80000)>>8) + ((XM_BIG_HIGHLIGHT_1&0xfc00)>>5) + ((XM_BIG_HIGHLIGHT_1&0xf8)>>3))

#define RAND "Now playing a random song."
#define XM_TITLE "XinMusic"

int xm_main() {
    uint32_t song_index = max_rand(sizeof(songs) / sizeof(struct song_entry) - 1);
    draw_background(XM_SECOND_COLOR);
    draw_square_size(center_x(160, vbe_info->width), 20, 40, 160, XM_MAIN_COLOR);
    draw_string(XM_TITLE, center_x(string_len(XM_TITLE) * 8, vbe_info->width), 32, XM_HIGHLIGHT_1);
    draw_square_size(center_x(190, vbe_info->width), 80, 80, 190, XM_MAIN_COLOR);
    draw_string(songs[song_index].song_name, center_x(string_len(songs[song_index].song_name) * 8, vbe_info->width), 80 + 16, XM_HIGHLIGHT_2);
    draw_string(songs[song_index].porter, center_x(string_len(songs[song_index].porter) * 8, vbe_info->width), 80 + 40, XM_HIGHLIGHT_1);
    draw_string(RAND, center_x(string_len(RAND) * 8, vbe_info->width), vbe_info->height / 2, XM_HIGHLIGHT_1);
    play_song_by_index(song_index);
    exit_to_main();
    return 0;
}