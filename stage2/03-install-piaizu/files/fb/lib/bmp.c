#include "container.h"
#include "framebuffer.h"
#include "log.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*
    offset	size	description
    00     	2  	signature, must be 4D42 hex
    02     	4  	size of BMP file in bytes (unreliable)
    06     	2  	reserved, must be zero
    08     	2  	reserved, must be zero
    10     	4  	offset to start of image data in bytes
    14     	4  	size of BITMAPINFOHEADER structure, must be 40.
    18     	4  	image width in pixels
    22     	4  	image height in pixels
    26     	2  	number of planes in the image, must be 1.
    28     	2  	number of bits per pixel (1, 4, 8, or 24)
    30     	4  	compression type (0=none, 1=RLE-8, 2=RLE-4)
    34     	4  	size of image data in bytes (including padding)
    38     	4  	horizontal resolution in pixels per meter (unreliable)
    42     	4  	vertical resolution in pixels per meter (unreliable)
    46     	4  	number of colors in image, or zero
    50     	4  	number of important colors, or zero
*/

// clang-format off
#if GRAYSCALE_LUMINOSITY

    #define GRAY_SCALE(rgb) \
        (((((((rgb)     & 0x000000FF) * 7)      +                   /*B*/ \
        ((((rgb) >> 8)  & 0x000000FF) * 72)     +                   /*G*/ \
        ((((rgb) >> 16) & 0x000000FF) * 21) ) / 100) << 8) &        /*R*/ \
        0x0000FF00)

#elif GRAYSCALE_AVERAGE

    #define GRAY_SCALE(rgb) \
        (((((((rgb)     & 0x000000FF))     +                /*B*/ \
        ((((rgb) >> 8)  & 0x000000FF))     +                /*G*/ \
        ((((rgb) >> 16) & 0x000000FF)) ) / 3) << 8) &       /*R*/ \
        0x0000FF00)

#else

    #define GRAY_SCALE(rgb) (rgb)

#endif
// clang-format on

#pragma pack(2)
typedef struct {

    uint8_t  signature[2];
    uint32_t filesize;
    uint16_t reserved_a;
    uint16_t reserved_b;
    uint32_t data_offset;
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t img_size_bytes;
    uint32_t x_res;
    uint32_t y_res;
    uint32_t n_colors;
    uint32_t n_important_colors;

} bmp_header_t;

int bmp_open_path(const char *path, const char *file, gimp_bitmap_t **bitmap) {
    bmp_header_t   header;
    char           filepath[255];
    ssize_t        n;
    unsigned char *bmp_data = NULL;

    int fd;

    if (bitmap == NULL) {
        return -1;
    }
    sprintf(filepath, "%s/%s", path, file);
    fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        LOG("Couldn't open %s %m\n", filepath);
        return -1;
    }

    lseek(fd, 0L, SEEK_SET);

    n = read(fd, &header, sizeof(header));
    if (n == -1) {
        LOG("Error reading filepath: %m");
    }

    bmp_data = calloc(header.img_size_bytes + sizeof(gimp_bitmap_t), 1);
    if (bmp_data == NULL) {
        return -1;
    }

    *bitmap = calloc((header.height * (header.width + 2) * 4) + sizeof(gimp_bitmap_t), 1);
    if (*bitmap == NULL) {

        free(bmp_data);
        return -1;
    }

    (*bitmap)->width  = header.width;
    (*bitmap)->height = header.height;

    (*bitmap)->bytes_per_pixel = 4; /* We use RGBA in as STD */

    lseek(fd, header.data_offset, SEEK_SET);
    n = read(fd, bmp_data, header.img_size_bytes);
    if (n == -1) {
        LOG("Error reading file %m\n");
        return n;
    }
    close(fd);

#ifdef DBG
    LOG("\nFilename: %s\nsignature: %c%c \n"
        "filesize: %d\n"
        "reserved_a: %d\n"
        "reserved_b: %d\n"
        "data_offset: 0x%x\n"
        "header_size: %d\n"
        "width: %d\n"
        "height: %d\n"
        "planes: %d\n"
        "bits_per_pixel: %d\n"
        "compression: %d\n"
        "img_size_bytes: %d\n"
        "x_res: %d\n"
        "y_res: %d\n"
        "n_colors: %d\n"
        "n_important_colors: %d\n",
        filepath, header.signature[0], header.signature[1], header.filesize, header.reserved_a, header.reserved_b,
        header.data_offset, header.header_size, header.width, header.height, header.planes, header.bits_per_pixel,
        header.compression, header.img_size_bytes, header.x_res, header.y_res, header.n_colors,
        header.n_important_colors);
#endif
    {
        unsigned int x, y;
        int          padding = 0;

        if ((header.width * (header.bits_per_pixel / 8)) % 4) {
            padding = 4 - (header.width * (header.bits_per_pixel / 8)) % 4;
        }

        for (y = 0; y < ((*bitmap)->height); y++) {
            for (x = 0; x < (*bitmap)->width; x++) {

                uint32_t *src;
                uint32_t *dst;

                src = (uint32_t *)&bmp_data[(x * (header.bits_per_pixel / 8)) +
                                            ((((header.height - 1) - y) * header.width) * (header.bits_per_pixel / 8)) +
                                            (((header.height - 1) - y) * padding)];

                dst = (uint32_t *)&(*bitmap)->pixel_data[(y * ((*bitmap)->width) + x) * (*bitmap)->bytes_per_pixel];

                if (header.bits_per_pixel == 32) {
                    *dst = GRAY_SCALE((*src >> 8));
                } else {
                    *dst = GRAY_SCALE((*src));
                }
            }
        }
        free(bmp_data);
    }

    return 0;
}

int bmp_open(const char *file, gimp_bitmap_t **bitmap) {
    return bmp_open_path(".", file, bitmap);
}

void bmp_free(gimp_bitmap_t **bitmap) {
    if (bitmap) {
        free(*bitmap);
        *bitmap = NULL;
    }
}
