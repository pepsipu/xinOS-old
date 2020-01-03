#include "../utils/math.c"
#include "font_bitmap.h"

/*
#define SSFN_NOIMPLEMENTATION
#define SSFN_CONSOLEBITMAP_HICOLOR

#include "ssfn.h"
*/
struct vbe_mode_info {
    uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
    uint8_t window_a;			// deprecated
    uint8_t window_b;			// deprecated
    uint16_t granularity;		// deprecated; used while calculating bank numbers
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
    uint16_t pitch;			// number of bytes per horizontal line
    uint16_t width;			// width in pixels
    uint16_t height;			// height in pixels
    uint8_t w_char;			// unused...
    uint8_t y_char;			// ...
    uint8_t planes;
    uint8_t bpp;			// bits per pixel in this mode
    uint8_t banks;			// deprecated; total number of banks in this mode
    uint8_t memory_model;
    uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
    uint8_t image_pages;
    uint8_t reserved0;

    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color_attributes;

    uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
    uint8_t reserved1[206];
} __attribute__ ((packed));

struct vbe_mode_info *vbe_info = (struct vbe_mode_info *) 0x8000;

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

void draw_string(char *s, uint16_t x, uint16_t y, uint16_t color) {
    int len = string_len(s);
    for (int i = 0; i < len; i++) {
        draw_char(s[i], x + (i*8), y, color);
    }
}

void test() {
    draw_background(0xe9ef);
    draw_string("please work!", 0, 0, 0);
}