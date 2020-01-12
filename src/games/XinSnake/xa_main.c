#include <kernel/vesa/graphics.c>
#include <music/music_list.h>
#include <kernel/utils/misc.c>
#include <kernel/utils/bmp_loader.c>
#include <kernel/utils/rand.c>

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
#define GAME_OVER "You lose!"

#define MOVE_AMOUNT 32

/*
 * The snake is a circular linked list. The head (the first box) is the "snake" object, it contains a pointer to the tail.
 * The tail's forward pointer will be all the mid body segments, which will end at the head once more.
 */
uint8_t volatile direction = 1; // 0 = north, 1 = east, 2 = south, 3 = west

struct snake_node {
    struct snake_node *next; // next piece of body
    int16_t x;
    int16_t y;
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
        // if statements prevent the snake from flipping directions
        case 'w':
            if (direction != 2) {
                direction = 0;
            }
            break;
        case 'd':
            if (direction != 3) {
                direction = 1;
            }
            break;
        case 's':
            if (direction != 0) {
                direction = 2;
            }
            break;
        case 'a':
            if (direction != 1) {
                direction = 3;
            }
            break;
        case 'q':
            exit_flag = 1;
        default:
            break;
    }
}

void game_over() {
    draw_string(GAME_OVER, center_x(string_len(GAME_OVER) * 8, vbe_info->width), 200, 0xb000);
    wait(1000);
}

// see if snake hit itself
int check_overlap() {
    struct snake_node *ptr = snake.next;
    while (ptr != &snake) {
        if (ptr->x == snake.x && ptr->y == snake.y) {
            return 1;
        }
        ptr = ptr->next;
    }
    return 0;
}

int xa_main() {
    draw_background(XA_MAIN_COLOR);
    draw_string(TITLE, center_x(string_len(TITLE) * 8, vbe_info->width), 50, XA_HIGHLIGHT_2);
    draw_string(SUBTITLE, center_x(string_len(SUBTITLE) * 8, vbe_info->width), 70, XA_HIGHLIGHT_1);
    draw_string(LOADING, center_x(string_len(LOADING) * 8, vbe_info->width), 200, XA_HIGHLIGHT_1);
    draw_line(70, 0, 70, vbe_info->height, XA_SECOND_COLOR, 20);
    draw_line(vbe_info->width - 70 - 20, 0, vbe_info->width - 70 - 20, vbe_info->height, XA_SECOND_COLOR, 20);
    key_down_handler = xa_key_handler;
    snake.x = 0;
    snake.y = 0;
    direction = 1;
    wait(2000);
    fruit.x = max_rand(vbe_info->width / MOVE_AMOUNT - 1); // will multiply by 32 later, but this is needed to ensure the fruit is aligned
    fruit.y = max_rand(vbe_info->height / MOVE_AMOUNT - 1); // will multiply by 32 later, but this is needed to ensure the fruit is aligned
    while (1) {
        draw_background(0);
        draw_square_size(fruit.x * MOVE_AMOUNT, fruit.y * MOVE_AMOUNT, MOVE_AMOUNT, MOVE_AMOUNT, 0xb000);
        struct snake_node *ptr = snake.next; // tail of snake
        // move segments of snake
        while (ptr != &snake){
            ptr->x = ptr->next->x;
            ptr->y = ptr->next->y;
            draw_square_size(ptr->x, ptr->y, MOVE_AMOUNT, MOVE_AMOUNT, 0xffff);
            ptr = ptr->next;
        }
        // move head
        switch (direction) {
            case 0:
                // move up, remember that the cartesian plane is flipped on the x axis!
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
        if (vbe_info->width < snake.x + MOVE_AMOUNT || snake.x < 0 || vbe_info->height < snake.y + MOVE_AMOUNT || snake.y < 0 || exit_flag || check_overlap()) {
            game_over();
            ptr = snake.next; // free all segments from tail to head
            while (ptr != &snake) {
                free(ptr);
                ptr = ptr->next;
            }
            snake.next = &snake;
            exit_flag = 0;
            exit_to_main();
            return 1;
        }
        if (snake.x == fruit.x * MOVE_AMOUNT && snake.y == fruit.y * MOVE_AMOUNT) {
            beep(200, 200, 0);
            fruit.x = max_rand(vbe_info->width / MOVE_AMOUNT - 1); // will multiply by 32 later, but this is needed to ensure the fruit is aligned
            fruit.y = max_rand(vbe_info->height / MOVE_AMOUNT - 1); // will multiply by 32 later, but this is needed to ensure the fruit is aligned
            // allocate new tail
            struct snake_node *new_tail = alloc(sizeof(struct snake_node));
            new_tail->next = snake.next; // point the new tail to the old tail
            snake.next = new_tail; // replace tail
            // x and y will be set in the next loop
        }
        draw_square_size(snake.x, snake.y, MOVE_AMOUNT, MOVE_AMOUNT, 0xffff);
        wait(150);
    }

}

