#include <kernel/vesa/graphics.c>
#include <music/music_list.h>
#include <kernel/utils/misc.c>
#include <kernel/utils/bmp_loader.c>

#define XA_BIG_MAIN_COLOR 0x363333 // 0x2f3032
#define XA_BIG_SECOND_COLOR 0x272121 // 0x383a56
#define XA_BIG_HIGHLIGHT_2 0xe16428 // 0xb0a565
#define XA_BIG_HIGHLIGHT_1 0xf6e9e9 // 0xede68a

// shift the RBG888 colors into RBG565, with a small loss of detail
#define XA_MAIN_COLOR (((XA_BIG_MAIN_COLOR&0xf80000)>>8) + ((XA_BIG_MAIN_COLOR&0xfc00)>>5) + ((XA_BIG_MAIN_COLOR&0xf8)>>3))
#define XA_SECOND_COLOR (((XA_BIG_SECOND_COLOR&0xf80000)>>8) + ((XA_BIG_SECOND_COLOR&0xfc00)>>5) + ((XA_BIG_SECOND_COLOR&0xf8)>>3))
#define XA_HIGHLIGHT_2 (((XA_BIG_HIGHLIGHT_2&0xf80000)>>8) + ((XA_BIG_HIGHLIGHT_2&0xfc00)>>5) + ((XA_BIG_HIGHLIGHT_2&0xf8)>>3))
#define XA_HIGHLIGHT_1 (((XA_BIG_HIGHLIGHT_1&0xf80000)>>8) + ((XA_BIG_HIGHLIGHT_1&0xfc00)>>5) + ((XA_BIG_HIGHLIGHT_1&0xf8)>>3))

#define TITLE "XinSnake"
#define SUBTITLE "A game to demonstrate the abilities of xinOS"
#define LOADING "Tip: (Q to quit). Now loading..."

#define MOVE_AMOUNT 16

/*
 * The snake is a circular linked list. The head (the first box) is the "snake" object, it contains a pointer to the tail.
 * The tail's forward pointer will be all the mid body segments, which will end at the head once more.
 */
uint8_t volatile direction = 1;

struct snake_node {
    uint8_t direction; // 0 = north, 1 = east, 2 = south, 3 = west
    struct snake_node *next; // next piece of body
    uint16_t x;
    uint16_t y;
} snake = {
        .next = &snake,
        .x = 0,
        .y = 0
};

struct {
    uint16_t x;
    uint16_t y;
} fruit = {};

uint8_t volatile exit_flag = 0;

void xa_key_handler(char key) {
    switch (key) {
        // w, d, s, a for setting snake position
        case 'w':
            snake.direction = 0;
            break;
        case 'd':
            snake.direction = 1;
            break;
        case 's':
            snake.direction = 2;
            break;
        case 'a':
            snake.direction = 3;
            break;
        case 'q':
            exit_flag = 1;
        default:
            break;
    }
}

int xa_main() {

    draw_background(XA_MAIN_COLOR);
    draw_string(TITLE, center_x(string_len(TITLE) * 8, vbe_info->width), 50, XA_HIGHLIGHT_2);
    draw_string(SUBTITLE, center_x(string_len(SUBTITLE) * 8, vbe_info->width), 70, XA_HIGHLIGHT_1);
    draw_string(LOADING, center_x(string_len(LOADING) * 8, vbe_info->width), 200, XA_HIGHLIGHT_1);
    draw_line(70, 0, 70, vbe_info->height, XA_SECOND_COLOR, 20);
    draw_line(vbe_info->width - 70 - 20, 0, vbe_info->width - 70 - 20, vbe_info->height, XA_SECOND_COLOR, 20);
    key_down_handler = xa_key_handler;
    direction = 1;
    wait(2000);
    while (!exit_flag) {
        draw_background(0);
        struct snake_node *ptr = snake.next; // tail of snake
        do {
            ptr->x = ptr->next->x;
            ptr->y = ptr->next->y;
            draw_square_size(ptr->x, ptr->y, MOVE_AMOUNT, MOVE_AMOUNT, 0xffff);
            ptr = ptr->next;
        } while (ptr->next != &snake);
        switch (direction) {
            case 0:
                // move up, remember that the cartesian plane is flipped on the y axis!
                snake.y -= MOVE_AMOUNT;
                break;
            case 1:
                // move right
                snake.x += MOVE_AMOUNT;
                break;
            case 2:
                // move down
                snake.y += MOVE_AMOUNT;
                break;
            case 3:
                // move left
                snake.x -= MOVE_AMOUNT;
            default:
                break;
        }
        wait(150);
    }
    exit_flag = 0;
    exit_to_main();
    return 1;
}