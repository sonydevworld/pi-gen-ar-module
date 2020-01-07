#pragma once
#include "container.h"

/**
 *
 * @param file filename to open, like logo.bmp
 * @param bitmap bitmap that later can be used in a container
 * @return Returns 0 on success, -1 on failure.
 *
 *      Opens a .bmp file, decodes it and creates a gimp_bitmap_t bitmap
 *      that can be printed in a container.
 */
int bmp_open(const char *file, gimp_bitmap_t **bitmap);

/**
 * @param path path to file
 * @param file filename to open, like logo.bmp
 * @param bitmap bitmap that later can be used in a container
 * @return Returns 0 on success, -1 on failure.
 *
 *      Opens a .bmp file, decodes it and creates a gimp_bitmap_t bitmap
 *      that can be printed in a container.
 */
int bmp_open_path(const char *path, const char *file, gimp_bitmap_t **bitmap);

/**
 *
 * @param bitmap bitmap to destroy and free
 *
 *      Frees the bitmap and sets the pointer to null.
 */
void bmp_free(gimp_bitmap_t **bitmap);
