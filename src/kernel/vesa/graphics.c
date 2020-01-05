#ifndef GRAPHICS
#define GRAPHICS
#include "../utils/time.c"
#include "../allocator.c"
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

void draw_line(uint16_t _x1, uint16_t _y1, uint16_t x2, uint16_t y2, uint16_t color, uint8_t thickness) {
    int y1 = _y1;
    int x1 = _x1;
    int dx = abs(x2-x1), sx = x1<x2 ? 1 : -1;
    int dy = abs(y2-y1), sy = y1<y2 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;

    for(;;){
        draw_pixel(x1, y1, color);
        if (x1==x2 && y1==y2) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x1 += sx; }
        if (e2 < dy) { err += dx; y1 += sy; }
    }
    if (thickness > 1) {
        draw_line(_x1 + 1, _y1, x2 + 1, y2, color, --thickness);
    }
}

void draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color, uint8_t thickness) {
    draw_line(x1, y1, x2, y2, color, thickness);
    draw_line(x2, y2, x3, y3, color, thickness);
    draw_line(x3, y3, x1, y1, color, thickness);
}


void draw_string(char *s, uint16_t x, uint16_t y, uint16_t color) {
    int len = string_len(s);
    for (int i = 0; i < len; i++) {
        draw_char(s[i], x + (i*8), y, color);
    }
}

void outline_circle(uint16_t xc, uint16_t yc, uint16_t x, uint16_t y, uint16_t color) {
    draw_pixel(xc+x, yc+y, color);
    draw_pixel(xc-x, yc+y, color);
    draw_pixel(xc+x, yc-y, color);
    draw_pixel(xc-x, yc-y, color);
    draw_pixel(xc+y, yc+x, color);
    draw_pixel(xc-y, yc+x, color);
    draw_pixel(xc+y, yc-x, color);
    draw_pixel(xc-y, yc-x, color);
}

// bresenham's algorithm, thanks geeksforgeeks
void draw_circle(uint16_t xc, uint16_t yc, uint16_t r, uint16_t color) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    outline_circle(xc, yc, x, y, color);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else d = d + 4 * x + 6;
        outline_circle(xc, yc, x, y, color);
    }
}

uint16_t center_x(uint16_t len, uint16_t container) {
    return (container / 2) - (len / 2);
}

#endif