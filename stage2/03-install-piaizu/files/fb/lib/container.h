#pragma once

#include "framebuffer.h"
#include <stdlib.h>

typedef uint8_t rgba_t[4];

#define LMX_PIXEL_WIDTH 419
#define LMX_PIXEL_HEIGHT 138

#define LMX_W LMX_PIXEL_WIDTH
#define LMX_H LMX_PIXEL_HEIGHT

typedef struct {
    int          handle;
    int          x;
    int          y;
    unsigned int width;
    unsigned int height;
    int          bits_per_pixel;
    int          padding;
    bool         transparent;
    bool         visible;
    char         frame;
    size_t       framesize_mem;
    rgba_t       bg_color;
    char *       description;
    uint8_t (*data)[];
} container_t;

typedef struct {
    size_t      n;
    container_t c[];
} containers_t;

#pragma pack(2)
typedef struct {
    unsigned int  width;
    unsigned int  height;
    unsigned int  bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    unsigned char pixel_data[];
} gimp_bitmap_t;

/**
 *
 *     Creates a new set of containers, allocates memory,
 *     reads out the current framebuffer size etc..
 */
void container_new(containers_t *containers);

/**
 *
 * @param container Container to fill with bitmap
 * @param x Start filling at offset x
 * @param y Start filling at offset y
 * @param bm bitmap image to fill
 * @param transparent Dark pixels are not overwriting background for
 * transparancy
 *
 *      Calls container_bitmap_at_xy with x,y set to 0.
 *
 *      Places a bitmap image at coordinates x,y in container at index
 * "container"
 */
int container_bitmap_at_xy(unsigned int container, int x, int y, gimp_bitmap_t *bm, bool transparent);

/**
 *
 * @param container Container to fill with bitmap
 * @param bm bitmap image to fill
 * @param transparent Dark pixels are not overwriting background for
 * transparancy
 *
 *      Calls container_bitmap_at_xy with x,y set to 0.
 */
int container_fill_bitmap(unsigned int container, gimp_bitmap_t *bm, bool transparent);

/**
 *
 *       Will compile all containers into container 0 which is a copy of the
 *       framebuffer, and then the data is set to the real framebuffer
 *       to be displayed on the screen.
 */
int container_compile();

/**
 * @param container Container to show/hide
 * @param visible Set to true to show and false hide.
 *
 *       Set a container to be visible or hidden.
 *       Content of container is not destroyed
 */
void container_visible(unsigned int container, bool visible);

/**
 * @param container Container to move
 * @param delta_x Move steps in x axis
 * @param delta_y Move steps in y axis
 *
 *      Move container with delta_x and delta_y
 */
void container_move(unsigned int container, int delta_x, int delta_y);

/**
 * @param container Container to reset to inital value
 *
 *      Resets the container to initial state. Destroys
 *      all contents.
 */
void container_reset(unsigned int container);

/**
 *
 *      Closes the framebuffer, frees all memory used for containers etc.
 *      Use this on exit.
 */
void container_close();

/**
 * @param container Container to draw in
 * @param from_x Start x point of line
 * @param from_y Start y point of line
 * @param to_x End x point of line
 * @param to_y End y point of line
 *
 * @return 0 on success, -1 on error.
 *
 * This function will draw a line in a container
 */

int container_draw_line(
    unsigned int container, unsigned int from_x, unsigned int from_y, unsigned int to_x, unsigned int to_y);

/**
 *
 * @return returns amount of defined containers.
 */
int container_get_n(void);

/**
 * @param container Container to container_get
 *
 * @return pointer to container.
 */
container_t *container_get(int container);
