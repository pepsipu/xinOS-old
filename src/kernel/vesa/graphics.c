#ifndef GRAPHICS
#define GRAPHICS

#include "vesa.c"

void draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    *((uint16_t *)(y * vbe_info->pitch + (x * (vbe_info->bpp/8)) + vbe_info->framebuffer)) = color;
}

void draw_background(uint16_t color) {
    memset_word((void *) vbe_info->framebuffer, color, vbe_info->height * vbe_info->pitch);
}

void draw_square_size(uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint16_t color) {
    for (int i = 0; i < height; i++) {
        for (int k = 0; k < width; k++) {
            draw_pixel(x + k, y + i, color);
        }
    }
}

void draw_square_coords(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    draw_square_size(x1, y1, abs(y2 - y1), abs(x2 - x1), color);
}

void draw_char(char c, uint16_t x, uint16_t y, uint16_t color) {
    size_t font_off = c * 16;

    for (uint32_t ny = 0; ny < 16; ny++) {
        for (uint32_t nx = 0; nx < 8; nx++) {
            if (__font_bitmap__[font_off + ny] & (1 << (8 - nx))) {
                draw_pixel(x + nx, y + ny, color);
            }
        }
    }
}

void draw_triangle(uint16_t x, uint16_t y, uint16_t color) {

}

void draw_string(char *s, uint16_t x, uint16_t y, uint16_t color) {
    int len = string_len(s);
    for (int i = 0; i < len; i++) {
        draw_char(s[i], x + (i*8), y, color);
    }
}

#endif