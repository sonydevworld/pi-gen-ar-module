#include "framebuffer.h"
#include "log.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

static struct frame_buffer fb = {0};

struct frame_buffer new_frame_buffer() {

    fb.file_descriptor = open("/dev/fb0", O_RDWR);
    if (fb.file_descriptor == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }

    if (ioctl(fb.file_descriptor, FBIOGET_FSCREENINFO, &fb.fixed_info) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    if (ioctl(fb.file_descriptor, FBIOGET_VSCREENINFO, &fb.var_info) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    fb.width      = fb.var_info.xres;
    fb.height     = fb.var_info.yres;
    fb.linelength = fb.fixed_info.line_length;
    fb.padding    = (fb.fixed_info.line_length / (fb.var_info.bits_per_pixel / 8)) - (fb.var_info.xres);

    LOG("Framebuffer: (width %d, height %d) bpp %d, bytesize %d (%d) linelength "
        "%d, padding %d\n",
        fb.var_info.xres, fb.var_info.yres, fb.var_info.bits_per_pixel, fb.fixed_info.smem_len,
        fb.width * fb.height * fb.var_info.bits_per_pixel / 8, fb.fixed_info.line_length, fb.padding);

    fb.buffer_size = fb.fixed_info.smem_len;
    fb.buffer      = (char *)mmap(0, fb.buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb.file_descriptor, 0);

    if (fb.buffer == (void *)-1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(5);
    }

    memset(fb.buffer, 0x00, fb.buffer_size);

    /* Return a copy of the frambuffer struct */
    return fb;
}

void destroy_frame_buffer() {
    munmap(fb.buffer, fb.buffer_size);
    close(fb.file_descriptor);
}

void framebuffer_update(void *mem) {
    memcpy(fb.buffer, mem, fb.buffer_size);
}
