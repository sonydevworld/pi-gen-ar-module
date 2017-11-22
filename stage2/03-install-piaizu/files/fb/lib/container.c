#include "container.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHA 3
#define RED 2
#define GREEN 1
#define BLUE 0

#if DBG
#define FRAMES_VISIBLE(x) 1
#else
#define FRAMES_VISIBLE(x) x
#endif

static containers_t default_container_data = {
    .n = 2,
    .c =
        {
            {
                /* 0 master layer */
                .x              = 0,
                .y              = 0,
                .width          = LMX_PIXEL_WIDTH,
                .height         = LMX_PIXEL_HEIGHT,
                .bits_per_pixel = 32,
                .transparent    = false,
                .visible        = true,
                .frame          = FRAMES_VISIBLE(0),
                .bg_color       = {0x00, 0x00, 0x00, 0x00},
                .description    = "Master container, copy of HW framebuffer",
                .data           = NULL,
            },
            {
                /* 1 */
                .x              = 0,
                .y              = 0,
                .width          = LMX_PIXEL_WIDTH,
                .height         = LMX_PIXEL_HEIGHT,
                .bits_per_pixel = 32,
                .transparent    = false,
                .visible        = true,
                .frame          = FRAMES_VISIBLE(0),
                .bg_color       = {0x00, 0x00, 0x00, 0x00},
                .description    = "Full screen",
                .data           = NULL,
            },
        },
};

static containers_t *container_data;

#ifdef GREEN_MONOCHROME
#define RGB_BITMASK 0x0000FF00
#else
#define RGB_BITMASK 0x00FFFFFF
#endif

/* Static functions*/
int container_valid(unsigned int container) {
    if (container == 0 || container >= container_data->n) {
        return -1;
    } else {
        return 0;
    }
}

/* Public functions */
container_t *container_get(int container);

void container_reset(unsigned int container) {

    memset(container_get(container)->data, 0x00, container_get(container)->framesize_mem);

    if (container_get(container)->frame) {
        container_draw_line(container, 0, 0, container_get(container)->width - 1, 0);
        container_draw_line(container, 0, container_get(container)->height - 1, container_get(container)->width - 1,
            container_get(container)->height - 1);
        container_draw_line(container, 0, 1, 0, container_get(container)->height - 1);

        container_draw_line(container, container_get(container)->width - 1, 0, container_get(container)->width - 1,
            container_get(container)->height - 1);
    }
}

static inline int container_get_pixel(unsigned int c, int col, int row, uint32_t *rgba) {
    uint32_t *px;

    px = (uint32_t *)container_data->c[c].data;

    *rgba = px[(col + row * (container_data->c[c].width + container_data->c[c].padding))];

    return 0;
}

static inline int container_set_pixel(unsigned int c, int col, int row, uint32_t rgba) {

    uint32_t *px;

    px = (uint32_t *)container_data->c[c].data;

    px[(col + row * (container_data->c[c].width + container_data->c[c].padding))] = rgba & RGB_BITMASK;

    return 0;
}

void container_visible(unsigned int container, bool visible) {
    if (container_valid(container)) {
        return;
    }

#ifdef DBG
    LOG("Container: %d is %s\n", container, visible ? "Visible" : "Invisible");
#endif
    container_data->c[container].visible = visible;
    return;
}

int container_compile() {

    unsigned int c = 0;

    for (c = 1; c < container_data->n; c++) {
        if (!container_data->c[c].visible) {
            continue;
        }

        for (unsigned int row = 0; row < container_data->c[c].height; row++) {
            for (unsigned int col = 0; col < container_data->c[c].width; col++) {
                uint32_t rgba;

                container_get_pixel(c, col, row, &rgba);
                if (container_data->c[c].transparent) {
                    if ((rgba & 0x00FFFFFF) == 0x00) {
                        continue;
                    }
                }
#ifdef ROTATE_180
                container_set_pixel(0, (container_data->c[0].width - 1) - (container_data->c[c].x + col),
                    (container_data->c[0].height - 1) - (container_data->c[c].y + row), rgba);
#else
                container_set_pixel(0, container_data->c[c].x + col, container_data->c[c].y + row, rgba);
#endif
            }
        }
    }

    framebuffer_update(container_data->c[0].data);
    memset(container_data->c[0].data, 0x00,
        container_data->c[0].width * container_data->c[0].height * sizeof(uint8_t) * 4);
    return 0;
}

int container_bitmap_at_xy(unsigned int container, int x, int y, gimp_bitmap_t *bm, bool transparent) {
    unsigned int w;
    unsigned int h;

    uint32_t *px;

    if (container_valid(container)) {
        return -1;
    }
    if (bm == NULL) {
        return -1;
    }

    for (h = 0; h < bm->height; h++) {

        for (w = 0; w < bm->width; w++) {
            px = (uint32_t *)bm->pixel_data;
            if (transparent) {
                if ((px[(w + h * bm->width)] & 0x00FFFFFF) == 0x00) {
                    continue;
                }
            }

            container_set_pixel(container, w + x, y + h, px[(w + h * bm->width)]);
        }
    }

    return 0;
}

int container_fill_bitmap(unsigned int container, gimp_bitmap_t *bm, bool transparent) {
    return container_bitmap_at_xy(container, 0, 0, bm, transparent);
}

container_t *container_get(int container) {
    return &container_data->c[container];
}
int container_get_n(void) {
    return container_data->n;
}

void container_close() {
    unsigned int i;
    for (i = 0; i < container_data->n; i++) {
        container_t *l = &container_data->c[i];
        if (l->data) {
            free(l->data);
        }
    }
    free(container_data);
    destroy_frame_buffer();
}

void container_new(containers_t *containers) {
    /* Initialise all containers */
    unsigned int        i;
    struct frame_buffer fb;
    size_t              total_bytes = 0;

    fb = new_frame_buffer();

    if (containers == NULL) {
        size_t s = sizeof(containers_t) + default_container_data.n * sizeof(container_t);
        container_data = malloc(s);
        memcpy(container_data, &default_container_data, s);

    } else {
        size_t s = sizeof(containers_t) + containers->n * sizeof(container_t);

        container_data = malloc(s);
        memcpy(container_data, containers, s);
    }

    for (i = 0; i < container_data->n; i++) {
        void *       data;
        size_t       allocsize = 0;
        container_t *l         = &container_data->c[i];

        /* Layer 0 must have same dimension than the framebuffer */
        if (i == 0) {
            allocsize  = fb.buffer_size;
            l->width   = fb.width;
            l->height  = fb.height;
            l->padding = fb.padding;
        } else {
            allocsize  = (l->width) * l->height * (fb.var_info.bits_per_pixel / 8);
            l->padding = 0;
        }

        data = calloc(allocsize, 1); /** todo Do not allocate this amount for each container. */

        l->framesize_mem = allocsize;
        total_bytes += l->framesize_mem;
        LOG("Container %02d: % 5d x% 5d bits/pix %d Visible: %s Transparent: %s "
            "size: % 8zd bytes padding: %2d %s\n",
            i, l->width, l->height, fb.var_info.bits_per_pixel, l->visible ? "True " : "False",
            l->transparent ? "True " : "False", l->framesize_mem, l->padding, l->description);

        /* All containers must have same color format, at least to start with */
        l->bits_per_pixel = fb.var_info.bits_per_pixel;
        l->data           = data;

        container_reset(i);
    }
    LOG("Total bytes allocated to %zd containers is: %zd bytes\n", container_data->n - 1, total_bytes);
}

void container_move(unsigned int container, int delta_x, int delta_y) {
    if (container_valid(container)) {
        return;
    }
    container_data->c[container].x += delta_x;
    container_data->c[container].y += delta_y;
}

int container_draw_line(
    unsigned int container, unsigned int from_x, unsigned int from_y, unsigned int to_x, unsigned int to_y) {
    double       k;
    int          m;
    unsigned int x, y;
    uint32_t     rgba = 0x0;

    if (container_valid(container)) {
        return -1;
    }
    if (to_x >= container_data->c[container].width || to_y >= container_data->c[container].height ||
        from_x >= container_data->c[container].width || from_y >= container_data->c[container].height) {
        return -1;
    }

    rgba = 0x0000FF00;

    if ((to_x - from_x) == 0) {

        for (y = from_y; y < to_y; y++) {
            container_set_pixel(container, to_x, y, rgba);
        }

        return 0;
    }

    k = (double)to_y - from_y;
    k = k / (to_x - from_x);

    m = (to_y - k * to_x);

    for (x = from_x; x <= to_x; x++) {
        y = (k * x) + m;
        container_set_pixel(container, x, y, rgba);
    }

    return 0;
}
