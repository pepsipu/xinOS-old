#include "../../pic/hanlders/keyboard.c"
#include "../graphics.c"
#include "../../utils/time.c"

void load_main_menu() {
    draw_background(0x7775);
    draw_string("timer test", 0, 0, 0);
    draw_square_size(40, 10, 6, 3, 0);
    wait(3);
    draw_square_size(46, 10, 6, 3, 0);
    wait(3);
    draw_square_size(40, 18, 3, 9, 0);
    wait(3);
}