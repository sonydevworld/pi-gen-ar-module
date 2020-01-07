#include "text.h"
#include "bmp.h"
#include "container.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char           character;
    int            offset_col;
    int            offset_row;
    int            width;
    int            height;
    gimp_bitmap_t *font;
} font_character_t;

static font_character_t alphabet_tbl[] = {
    {.character = 'A', .offset_col = 0, .offset_row = 1, .width = 12, .height = 13, .font = NULL},
    {.character = 'B', .offset_col = 14, .offset_row = 1, .width = 8, .height = 13, .font = NULL},
    {.character = 'C', .offset_col = 25, .offset_row = 1, .width = 11, .height = 13, .font = NULL},
    {.character = 'D', .offset_col = 39, .offset_row = 1, .width = 11, .height = 13, .font = NULL},
    {.character = 'E', .offset_col = 53, .offset_row = 1, .width = 8, .height = 13, .font = NULL},
    {.character = 'F', .offset_col = 64, .offset_row = 1, .width = 9, .height = 13, .font = NULL},
    {.character = 'G', .offset_col = 73, .offset_row = 1, .width = 12, .height = 13, .font = NULL},
    {.character = 'H', .offset_col = 88, .offset_row = 1, .width = 13, .height = 13, .font = NULL},
    {.character = 'I', .offset_col = 102, .offset_row = 1, .width = 2, .height = 13, .font = NULL},
    {.character = 'J', .offset_col = 105, .offset_row = 1, .width = 6, .height = 16, .font = NULL},
    {.character = 'K', .offset_col = 114, .offset_row = 1, .width = 12, .height = 13, .font = NULL},
    {.character = 'L', .offset_col = 125, .offset_row = 1, .width = 8, .height = 13, .font = NULL},
    {.character = 'M', .offset_col = 135, .offset_row = 1, .width = 14, .height = 13, .font = NULL},
    {.character = 'N', .offset_col = 152, .offset_row = 1, .width = 10, .height = 13, .font = NULL},
    {.character = 'O', .offset_col = 164, .offset_row = 1, .width = 13, .height = 13, .font = NULL},
    {.character = 'P', .offset_col = 180, .offset_row = 1, .width = 10, .height = 13, .font = NULL},
    {.character = 'Q', .offset_col = 190, .offset_row = 1, .width = 13, .height = 15, .font = NULL},
    {.character = 'R', .offset_col = 205, .offset_row = 1, .width = 11, .height = 13, .font = NULL},
    {.character = 'S', .offset_col = 216, .offset_row = 1, .width = 11, .height = 13, .font = NULL},
    {.character = 'T', .offset_col = 227, .offset_row = 1, .width = 13, .height = 13, .font = NULL},
    {.character = 'U', .offset_col = 241, .offset_row = 1, .width = 11, .height = 13, .font = NULL},
    {.character = 'V', .offset_col = 251, .offset_row = 1, .width = 14, .height = 13, .font = NULL},
    {.character = 'W', .offset_col = 266, .offset_row = 1, .width = 16, .height = 13, .font = NULL},
    {.character = 'X', .offset_col = 285, .offset_row = 1, .width = 12, .height = 13, .font = NULL},
    {.character = 'Y', .offset_col = 297, .offset_row = 1, .width = 12, .height = 13, .font = NULL},
    {.character = 'Z', .offset_col = 310, .offset_row = 1, .width = 12, .height = 13, .font = NULL},
    /* Lower case */
    {.character = 'a', .offset_col = 1, .offset_row = 22, .width = 9, .height = 16, .font = NULL},
    {.character = 'b', .offset_col = 11, .offset_row = 22, .width = 10, .height = 16, .font = NULL},
    {.character = 'c', .offset_col = 22, .offset_row = 22, .width = 9, .height = 16, .font = NULL},
    {.character = 'd', .offset_col = 31, .offset_row = 22, .width = 9, .height = 16, .font = NULL},
    {.character = 'e', .offset_col = 41, .offset_row = 22, .width = 10, .height = 16, .font = NULL},
    {.character = 'f', .offset_col = 52, .offset_row = 22, .width = 7, .height = 16, .font = NULL},
    {.character = 'g', .offset_col = 59, .offset_row = 22, .width = 10, .height = 20, .font = NULL},
    {.character = 'h', .offset_col = 71, .offset_row = 22, .width = 8, .height = 16, .font = NULL},
    {.character = 'i', .offset_col = 82, .offset_row = 22, .width = 3, .height = 16, .font = NULL},
    {.character = 'j', .offset_col = 87, .offset_row = 22, .width = 4, .height = 20, .font = NULL},
    {.character = 'k', .offset_col = 92, .offset_row = 22, .width = 8, .height = 16, .font = NULL},
    {.character = 'l', .offset_col = 102, .offset_row = 22, .width = 4, .height = 16, .font = NULL},
    {.character = 'm', .offset_col = 107, .offset_row = 22, .width = 15, .height = 16, .font = NULL},
    {.character = 'n', .offset_col = 124, .offset_row = 22, .width = 8, .height = 16, .font = NULL},
    {.character = 'o', .offset_col = 134, .offset_row = 22, .width = 10, .height = 16, .font = NULL},
    {.character = 'p', .offset_col = 146, .offset_row = 22, .width = 10, .height = 20, .font = NULL},
    {.character = 'q', .offset_col = 155, .offset_row = 22, .width = 12, .height = 20, .font = NULL},
    {.character = 'r', .offset_col = 168, .offset_row = 22, .width = 7, .height = 16, .font = NULL},
    {.character = 's', .offset_col = 175, .offset_row = 22, .width = 8, .height = 16, .font = NULL},
    {.character = 't', .offset_col = 183, .offset_row = 22, .width = 8, .height = 16, .font = NULL},
    {.character = 'u', .offset_col = 191, .offset_row = 22, .width = 9, .height = 16, .font = NULL},
    {.character = 'v', .offset_col = 201, .offset_row = 22, .width = 10, .height = 16, .font = NULL},
    {.character = 'w', .offset_col = 213, .offset_row = 22, .width = 12, .height = 16, .font = NULL},
    {.character = 'x', .offset_col = 227, .offset_row = 22, .width = 12, .height = 16, .font = NULL},
    {.character = 'y', .offset_col = 239, .offset_row = 22, .width = 10, .height = 20, .font = NULL},
    {.character = 'z', .offset_col = 250, .offset_row = 22, .width = 12, .height = 16, .font = NULL},
    {.character = '0', .offset_col = 0, .offset_row = 42, .width = 12, .height = 16, .font = NULL},
    {.character = '1', .offset_col = 13, .offset_row = 42, .width = 8, .height = 16, .font = NULL},
    {.character = '2', .offset_col = 23, .offset_row = 42, .width = 10, .height = 16, .font = NULL},
    {.character = '3', .offset_col = 34, .offset_row = 42, .width = 9, .height = 16, .font = NULL},
    {.character = '4', .offset_col = 44, .offset_row = 42, .width = 9, .height = 16, .font = NULL},
    {.character = '5', .offset_col = 55, .offset_row = 42, .width = 9, .height = 16, .font = NULL},
    {.character = '6', .offset_col = 66, .offset_row = 42, .width = 10, .height = 16, .font = NULL},
    {.character = '7', .offset_col = 78, .offset_row = 42, .width = 10, .height = 16, .font = NULL},
    {.character = '8', .offset_col = 89, .offset_row = 42, .width = 10, .height = 16, .font = NULL},
    {.character = '9', .offset_col = 99, .offset_row = 42, .width = 10, .height = 16, .font = NULL},
    {.character = '.', .offset_col = 230, .offset_row = 42, .width = 3, .height = 16, .font = NULL},
    {.character = '/', .offset_col = 234, .offset_row = 44, .width = 7, .height = 16, .font = NULL},
    {.character = ':', .offset_col = 248, .offset_row = 42, .width = 5, .height = 16, .font = NULL},

};

void text_close() {
    unsigned int i;
    for (i = 0; i < sizeof(alphabet_tbl) / sizeof(alphabet_tbl[0]); i++) {
        if (alphabet_tbl[i].font != NULL) {
            free(alphabet_tbl[i].font);
        } else {
            continue;
        }
    }
}

void text_init() {
    unsigned int i;
    size_t       charsize;
    int          x;
    int          y;
    uint32_t(*destfont)[];
    uint32_t(*srcfont)[];
    gimp_bitmap_t *alphabet;

    long int dest_px = 0;
    long int src_px  = 0;

    i = bmp_open("graphics/alphabet.bmp", &alphabet);
    if (i) {

        return;
    }

    for (i = 0; i < sizeof(alphabet_tbl) / sizeof(alphabet_tbl[0]); i++) {
        charsize             = alphabet_tbl[i].width * alphabet_tbl[i].height * sizeof(rgba_t);
        alphabet_tbl[i].font = calloc(charsize + sizeof(gimp_bitmap_t), 1);

        alphabet_tbl[i].font->width           = alphabet_tbl[i].width;
        alphabet_tbl[i].font->height          = alphabet_tbl[i].height;
        alphabet_tbl[i].font->bytes_per_pixel = sizeof(rgba_t);

        destfont = (uint32_t(*)[])alphabet_tbl[i].font->pixel_data;

        for (y = 0; y < alphabet_tbl[i].height; y++) {
            for (x = 0; x < alphabet_tbl[i].width; x++) {

                src_px = (alphabet_tbl[i].offset_row + y) * alphabet->width;
                src_px += alphabet_tbl[i].offset_col + x;

                srcfont = (uint32_t(*)[]) & alphabet->pixel_data;

                dest_px = y * alphabet_tbl[i].font->width;
                dest_px += x;

                if (((*srcfont)[src_px] & 0xFFFF00) != 00) {
                    (*destfont)[dest_px] = (*srcfont)[src_px];
                }
            }
        }
    }
    bmp_free(&alphabet);
}

#define OFFSET_LOWERCASE ('Z' - 'A') + 1
#define OFFSET_NUMBERS OFFSET_LOWERCASE + ('z' - 'a') + 1
#define OFFSET_SPECIAL OFFSET_NUMBERS + ('9' - '0') + 1

font_character_t *text_get_char_font(char c) {
    if (c >= 'a' && c <= 'z') {
        return &alphabet_tbl[(c - 'a') + OFFSET_LOWERCASE];
    } else if (c >= 'A' && c <= 'Z') {
        return &alphabet_tbl[c - 'A'];
    } else if (c >= '0' && c <= '9') {
        return &alphabet_tbl[(c - '0') + OFFSET_NUMBERS];
    } else if (c >= '.' && c <= '/') {
        return &alphabet_tbl[(c - '.') + OFFSET_SPECIAL];
    } else if (c == ':') {
        return &alphabet_tbl[(c - ':') + OFFSET_SPECIAL + 2];
    }
    return NULL;
}

gimp_bitmap_t *text_get_char_bitmap(char c) {
    font_character_t *f;
    f = text_get_char_font(c);
    if (f) {
        return f->font;
    }
    return NULL;
}

void text_put(int container, char *string) {
    int               strsize;
    int               i;
    font_character_t *c;
    int               x = 0;
    int               y = 2;

    strsize = strlen(string);
    for (i = 0; i < strsize; i++) {
        if (string[i] == ' ') {
            x = x + 7;
            continue;
        }
        if (string[i] == '\n') {
            y = y + 20;
            x = 0;
            continue;
        }
        c = text_get_char_font(string[i]);
        if (c == NULL) {
            continue;
        }

        container_bitmap_at_xy(container, x, y, c->font, true);
        x += c->width;
        x++;
        x++;
    }
}
