#ifndef MAIN_MENU
#define MAIN_MENU

#include <kernel/pic/hanlders/keyboard.c>
#include <kernel/vesa/graphics.c>
#include <games/game_list.h>
#include <kernel/sound/pc_speaker.c>
#include <kernel/utils/strings.c>

// two palettes, one is icey/cold and the other is sandy/warm
// default to icey

#define BIG_MAIN_COLOR 0x363333 //0x222831 // 0x2f3032
#define BIG_SECOND_COLOR 0x272121 // 0x393e46 // 0x383a56
#define BIG_HIGHLIGHT_2 0xf6e9e9 // 0x00adb5 // 0xb0a565
#define BIG_HIGHLIGHT_1 0xe16428 // 0xeeeeee // 0xede68a

// shift the RBG888 colors into RBG565, with a small loss of detail
#define MAIN_COLOR (((BIG_MAIN_COLOR&0xf80000)>>8) + ((BIG_MAIN_COLOR&0xfc00)>>5) + ((BIG_MAIN_COLOR&0xf8)>>3))
#define SECOND_COLOR (((BIG_SECOND_COLOR&0xf80000)>>8) + ((BIG_SECOND_COLOR&0xfc00)>>5) + ((BIG_SECOND_COLOR&0xf8)>>3))
#define HIGHLIGHT_2 (((BIG_HIGHLIGHT_2&0xf80000)>>8) + ((BIG_HIGHLIGHT_2&0xfc00)>>5) + ((BIG_HIGHLIGHT_2&0xf8)>>3))
#define HIGHLIGHT_1 (((BIG_HIGHLIGHT_1&0xf80000)>>8) + ((BIG_HIGHLIGHT_1&0xfc00)>>5) + ((BIG_HIGHLIGHT_1&0xf8)>>3))

#define TITLE_MSG "xinOS: XIN isn't NES!"
#define SUBTITLE_MSG "An operating system to bring back the days of the NES."

#define GAME_Y_POS 150
#define GAME_DISPLAY_COUNT 3
#define GAME_PADDING 40
#define GAME_HEIGHT 100
#define GAME_WIDTH 150
#define GAMES_LENGTH sizeof(games) / sizeof(struct game)

volatile uint16_t action = 0;

volatile uint8_t game_index = 0;
volatile uint8_t game_point = 0;

void draw_game_pointer() {
    int remaining_space = vbe_info->width - ((GAME_PADDING + GAME_WIDTH) * GAME_DISPLAY_COUNT);
    draw_square_size((remaining_space / 2) + GAME_PADDING / 2, GAME_Y_POS + GAME_HEIGHT, 30, (GAME_PADDING + GAME_WIDTH) * GAME_DISPLAY_COUNT - GAME_PADDING, MAIN_COLOR);
    int game_start_pos = (remaining_space / 2) + ((GAME_PADDING + GAME_WIDTH) * game_point) + GAME_PADDING / 2;
    draw_triangle(game_start_pos + (GAME_WIDTH / 2), GAME_Y_POS + GAME_HEIGHT + 5, game_start_pos + (GAME_WIDTH / 2) + 15, GAME_Y_POS + GAME_HEIGHT + 25, game_start_pos + (GAME_WIDTH / 2) - 15, GAME_Y_POS + GAME_HEIGHT + 25, 0, 1);
}

void load_games() {
    // this code is a mess, it's mostly to center the boxes with variable length padding. Don't touch, it's fragile.
    int remaining_space = vbe_info->width - ((GAME_PADDING + GAME_WIDTH) * GAME_DISPLAY_COUNT);
    draw_square_size((remaining_space / 2) + GAME_PADDING / 2, GAME_Y_POS + GAME_HEIGHT, GAME_DISPLAY_COUNT * ((GAME_PADDING + GAME_WIDTH) + GAME_PADDING / 2), GAME_WIDTH + GAME_PADDING, MAIN_COLOR);
    for (int i = 0; i < GAME_DISPLAY_COUNT; i++) {
        int game_screen_space = i * (GAME_PADDING + GAME_WIDTH) + GAME_PADDING / 2;
        draw_square_size(game_screen_space + (remaining_space / 2), GAME_Y_POS, GAME_HEIGHT, GAME_WIDTH, SECOND_COLOR);
        if (i < GAMES_LENGTH) {
            draw_string(games[i + game_index].name, center_x(string_len(games[i + game_index].name) * 8, GAME_WIDTH) + game_screen_space + (remaining_space / 2),GAME_Y_POS + 10, HIGHLIGHT_1);
            draw_string(games[i + game_index].author, center_x(string_len(games[i + game_index].author) * 8, GAME_WIDTH) + game_screen_space + (remaining_space / 2), GAME_Y_POS + 40, HIGHLIGHT_2);
        }
        // draw padding for debugging
        // draw_square_size(game_screen_space + remaining_space / 2 + GAME_WIDTH, 80, GAME_HEIGHT, GAME_PADDING,  HIGHLIGHT_2);
        draw_game_pointer();
    }
}



void key_press(char c) {
    if (c == 'd') {
        if (GAMES_LENGTH - GAME_DISPLAY_COUNT >= game_index + 1 && game_index + 1 > GAMES_LENGTH) {
            game_index++;
            load_games();
        } else if (game_point != 2) {
            game_point++;
            draw_game_pointer();
        } else {
            action |= 1;
        }
    } else if (c == 'a') {
        if (game_index - 1 >= 0) {
            game_index--;
            load_games();
        } else if (game_point != 0) {
            game_point--;
            draw_game_pointer();
        } else {
            action |= 1; // toggle bit 0, the bit for beeping
        }
    } else if (c == '\n') {
        action |= 2; // toggle bit 1, the bit for launching a game
    }

}

void load_main_menu() {
    menu_start:
    key_down_handler = key_press;
    draw_background(MAIN_COLOR);
    draw_string(TITLE_MSG, center_x(string_len(TITLE_MSG) * 8, vbe_info->width), 20, HIGHLIGHT_1);
    draw_string(SUBTITLE_MSG, center_x(string_len(SUBTITLE_MSG) * 8, vbe_info->width), 40, HIGHLIGHT_2);

    load_games();
    // draw arrows for games
    draw_triangle(10, GAME_Y_POS + (GAME_HEIGHT / 2), 30, GAME_Y_POS + 10, 30, GAME_Y_POS + GAME_HEIGHT - 10, SECOND_COLOR, 2);
    draw_triangle(vbe_info->width - 10, GAME_Y_POS + (GAME_HEIGHT / 2), vbe_info->width - 30, GAME_Y_POS + 10, vbe_info->width - 30, GAME_Y_POS + GAME_HEIGHT - 10, SECOND_COLOR, 2);
    // draw "a" and "d"
    draw_char('a', 25, GAME_Y_POS + 100, HIGHLIGHT_1);
    draw_char('d', vbe_info->width - 25, GAME_Y_POS + 100, HIGHLIGHT_1);
    /*
     * Because interrupts cannot call beeps and other actions (soft locks the os), interrupts can "request" a beep for
     * the main thread to call. This is done through the action variable. Each bit of the action variable is a different
     * action the main thread should perform on behalf of the interrupt
     */
    while (1) {
        asm volatile("hlt"); // wait until interrupt, specifically the keyboard interrupt
        if (action) {
            if (action == 0xff) { // quit action
                break;
            } else if (action & 1) { // beep request
                beep(100, 100, 0);
                action &= 0xfe; // disable beep request bit
            } else if (action & 2) { // launch game request
                action &= 0xfd; // disable launch game bit
                if (GAMES_LENGTH - 1>= game_index + game_point) {
                    games[game_point + game_index].init();
                    goto menu_start;
                }
            }
        }
    }
}

#endif