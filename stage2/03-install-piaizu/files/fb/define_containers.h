#pragma once

#if DBG
#define FRAME(x) 1
#else
#define FRAME(x) x
#endif

#if DBG
#define FRAMES_VISIBLE(x) 1
#else
#define FRAMES_VISIBLE(x) x
#endif

// clang-format off
/*            HANDLE,        X,   Y,          WIDTH, HEIGHT, TRANSPARENT, VISIBLE, SHOW_FRAME, DESCRIPTION) */
#define CONTAINER_DEFINE(x) \
 /* 00 */   x(FB_COPY      , 0   ,0         , LMX_W, LMX_H,  false, true, FRAME(0), "Master container, copy of HW framebuffer") \
 /* 01 */   x(FULL_SCREEN  , 0   ,0         , LMX_W, LMX_H,  false, true, FRAME(0), "Full screen, not used") \
 /* 02 */   x(SONY_LOGO    , 20  ,10        , 246  , 52,     true , true, FRAME(0), "Master container, copy of HW framebuffer") \
 /* 03 */   x(AP_A         , 360 , 1        , 57   , 70,     false, true, FRAME(0), "Access point symbol A") \
 /* 04 */   x(AP_B         , 360 , 1        , 57   , 70,     true , true, FRAME(0), "Access point symbol B") \
 /* 05 */   x(AP_C         , 360 , 1        , 57   , 70,     true , true, FRAME(0), "Access point symbol C") \
 /* 06 */   x(AP_D         , 360 , 1        , 57   , 70,     true , true, FRAME(0), "Access point symbol D") \
 /* 07 */   x(TEXT         , 375 ,75        , 30   , 30,     true , true, FRAME(0), "Text") \
 /* 08 */   x(WIFI_TEXTBOX , 20  ,80        , 280  , 48,     true , true, FRAME(0), "Connect to wifi textbox") \
 /* 09 */   x(KEY_INFO     , 290 ,10        , 65   , 62,     true , true, FRAME(0), "Key event information") \
 /* 10 */   x(WEB_MESSAGES , 0   ,LMX_H - 21, LMX_W, 20,     true , true, FRAME(0), "Web message area") \
 /* 11 */   x(POINTER      , 0   ,0         , LMX_W, LMX_H,  true , true, FRAME(0), "Cross-hair area")

#define CONTAINER_HANDLE_ENUM(HANDLE, X, Y, WIDTH, HEIGHT, TRANSPARENT, VISIBLE, SHOW_FRAME, DESCRIPTION) HANDLE,
#define CONTAINER_HANDLE_STR(HANDLE, X, Y, WIDTH, HEIGHT, TRANSPARENT, VISIBLE, SHOW_FRAME, DESCRIPTION) #HANDLE ,

typedef enum {
    CONTAINER_DEFINE(CONTAINER_HANDLE_ENUM)
    N_CONTAINERS
} container_handle_e;

#define CONTAINER_ARRAY(HANDLE, X, Y, WIDTH, HEIGHT, TRANSPARENT, VISIBLE, SHOW_FRAME, DESCRIPTION) \
    {                                                                  \
        .x              = X,                                           \
        .y              = Y,                                           \
        .width          = WIDTH,                             \
        .height         = HEIGHT,                            \
        .bits_per_pixel = 32,                                          \
        .transparent    = TRANSPARENT,                                 \
        .visible        = VISIBLE,                                     \
        .frame          = SHOW_FRAME,                                  \
        .bg_color       = {0x00, 0x00, 0x00, 0x00},                    \
        .description    = DESCRIPTION,                                 \
        .data           = NULL,                                        \
    },

#if 0

typedef struct {
    size_t n;
    container_t c[];
} containers_t;


static container_t containers[] = {
    CONTAINER_DEFINE(CONTAINER_ARRAY)
};

#endif

static containers_t containers = {
    .n = N_CONTAINERS,
    .c =
        {
            CONTAINER_DEFINE(CONTAINER_ARRAY)
        }
};
// clang-format on
