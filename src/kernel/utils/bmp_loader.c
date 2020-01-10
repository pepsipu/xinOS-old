#include <kernel/vesa/graphics.c>

struct file_header {
    uint16_t signature; // "BM", to signify that it's a bitmap file
    uint32_t file_size;
    uint32_t reserved; // must be 0
    uint32_t pixel_data;
} __attribute__((packed));

struct image_header {
    uint32_t header_size; // size of the image_header
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
    uint16_t planes; // image planes, always 1 for a bmp
    uint16_t bits_per_pixel; // bits per pixel, xinOS only allows 16 bit color
    uint16_t compression; // compression, xinOS only works with uncompressed (this should be 0)
    uint32_t image_size;
    uint32_t horiz_pref; // preferred resolution, ignored
    uint32_t vert_pref; // preferred resolution ignored
    uint32_t color_map_entries;
    uint32_t important_colors; // ignored, xinOS can display the colors needed without sacrificing some
};

void draw_bmp(uint8_t *bmp, uint16_t x, uint16_t y) {
    struct file_header *file = bmp;
    struct image_header *img = bmp + sizeof(struct file_header);
    uint16_t *pixels = bmp + file->pixel_data;
    for (uint32_t i = 0; i < img->height; i++) {
        for (uint32_t k = 0; k < img->width; k++) {
            draw_pixel(x + k, y + i, *(pixels + k + img->width * (img->height - i - 1))); // bmps are stored upside down
        }
    }
};