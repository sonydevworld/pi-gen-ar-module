#pragma once
#include <linux/fb.h>
#include <stdbool.h>
#include <stdint.h>

struct frame_buffer {
    int file_descriptor;

    struct fb_fix_screeninfo fixed_info;
    struct fb_var_screeninfo var_info;

    int padding;
    int linelength;
    int width;
    int height;

    long int buffer_size;
    char *   buffer;
};

struct frame_buffer new_frame_buffer();

void framebuffer_update(void *mem);

void destroy_frame_buffer();
