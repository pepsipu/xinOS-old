#include "../../pic/hanlders/keyboard.c"
#include "../graphics.c"
#include "../../../games/game_list.h"

// two palettes, one is icey/cold and the other is sandy/warm
// default to icey

#define BIG_MAIN_COLOR 0x222831 // 0x2f3032
#define BIG_SECOND_COLOR 0x393e46 // 0x383a56
#define BIG_HIGHLIGHT_2 0x00adb5 // 0xb0a565
#define BIG_HIGHLIGHT_1 0xeeeeee // 0xede68a

// shift the RBG888 colors into RBG565, with a small loss of detail
#define MAIN_COLOR (((BIG_MAIN_COLOR&0xf80000)>>8) + ((BIG_MAIN_COLOR&0xfc00)>>5) + ((BIG_MAIN_COLOR&0xf8)>>3))
#define SECOND_COLOR (((BIG_SECOND_COLOR&0xf80000)>>8) + ((BIG_SECOND_COLOR&0xfc00)>>5) + ((BIG_SECOND_COLOR&0xf8)>>3))
#define HIGHLIGHT_2 (((BIG_HIGHLIGHT_2&0xf80000)>>8) + ((BIG_HIGHLIGHT_2&0xfc00)>>5) + ((BIG_HIGHLIGHT_2&0xf8)>>3))
#define HIGHLIGHT_1 (((BIG_HIGHLIGHT_1&0xf80000)>>8) + ((BIG_HIGHLIGHT_1&0xfc00)>>5) + ((BIG_HIGHLIGHT_1&0xf8)>>3))

#define TITLE_MSG "xinOS: XIN isn't NES!"
#define SUBTITLE_MSG "An operating system to bring back the days of the NES."

#define GAME_DISPLAY_COUNT 3
#define GAME_PADDING 40
#define GAME_HEIGHT 100
#define GAME_WIDTH 150
#define GAMES_LENGTH sizeof(games) / sizeof(struct game)

volatile uint8_t game_index = 0;
volatile uint8_t game_point = 0;

void load_games() {
    int remaining_space = vbe_info->width - ((GAME_PADDING + GAME_WIDTH) * GAME_DISPLAY_COUNT);
    draw_square_size((remaining_space / 2) + GAME_PADDING / 2, 80 + GAME_HEIGHT, GAME_DISPLAY_COUNT * ((GAME_PADDING + GAME_WIDTH) + GAME_PADDING / 2), GAME_WIDTH + GAME_PADDING, MAIN_COLOR);
    for (int i = 0; i < GAME_DISPLAY_COUNT; i++) {
        int game_screen_space = i * (GAME_PADDING + GAME_WIDTH) + GAME_PADDING / 2;
        draw_square_size(game_screen_space + (remaining_space / 2), 80, GAME_HEIGHT, GAME_WIDTH, SECOND_COLOR);
        draw_string(games[i + game_index].name, center_x(string_len(games[i + game_index].name) * 8, GAME_WIDTH) + game_screen_space + (remaining_space / 2),90, HIGHLIGHT_1);
        draw_string(games[i + game_index].author, center_x(string_len(games[i + game_index].author) * 8, GAME_WIDTH) + game_screen_space + (remaining_space / 2), 130, HIGHLIGHT_2);
        // draw padding for debugging
        // draw_square_size(game_screen_space + remaining_space / 2 + GAME_WIDTH, 80, GAME_HEIGHT, GAME_PADDING,  HIGHLIGHT_2);
    }
}

void key_press(char c) {
    if (c == 'd') {
        if (GAMES_LENGTH - GAME_DISPLAY_COUNT >= game_index + 1) {
            game_index++;
            load_games();
        }
    }
    if (c == 'a') {
        if (game_index - 1 >= 0) {
            game_index--;
            load_games();
        }
    }

}

void load_main_menu() {
    key_down_handler = key_press;
    draw_background(MAIN_COLOR);
    draw_string(TITLE_MSG, center_x(string_len(TITLE_MSG) * 8, vbe_info->width), 20, HIGHLIGHT_1);
    draw_string(SUBTITLE_MSG, center_x(string_len(SUBTITLE_MSG) * 8, vbe_info->width), 40, HIGHLIGHT_2);

    load_games();
    // draw arrows for games
    draw_triangle(10, 80 + (GAME_HEIGHT / 2), 30, 80 + 10, 30, 80 + GAME_HEIGHT - 10, SECOND_COLOR, 2);
    draw_triangle(vbe_info->width - 10, 80 + (GAME_HEIGHT / 2), vbe_info->width - 30, 80 + 10, vbe_info->width - 30, 80 + GAME_HEIGHT - 10, SECOND_COLOR, 2);
    // draw "a" and "d"
    draw_char('a', 25, 180, HIGHLIGHT_1);
    draw_char('d', vbe_info->width - 25, 180, HIGHLIGHT_1);

}



/*void load_main_menu() {
    draw_background(0x7775);
    draw_circle(120, 120, 100, 0);
    draw_line(70, 80, 70, 120, 0, 3);
    draw_line(120, 80, 120, 120, 0, 3);
    draw_line(90, 160, 70, 130, 0, 3);
    draw_line(90, 160, 120, 130, 0, 3);
    draw_triangle(300, 400, 325, 325, 350, 350, 0, 1);
    draw_triangle(200, 220, 245, 278, 245, 220, 0, 1);
    draw_string("i require your soul", 20, 240, 0);
}*/