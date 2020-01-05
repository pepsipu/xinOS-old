#include "../../pic/hanlders/keyboard.c"
#include "../graphics.c"
#include "../../utils/time.c"

void load_main_menu() {
    draw_background(0x7775);
    draw_circle(120, 120, 100, 0);
    draw_line(70, 80, 70, 120, 0, 3);
    draw_line(120, 80, 120, 120, 0, 3);
    draw_line(90, 160, 70, 130, 0, 3);
    draw_line(90, 160, 120, 130, 0, 3);
    draw_string("i require your soul", 20, 240, 0);
}