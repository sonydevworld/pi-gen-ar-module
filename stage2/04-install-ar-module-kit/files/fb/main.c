#include "bmp.h"
#include "container.h"
#include "define_containers.h"
#include "fwatch.h"
#include "keys.h"
#include "log.h"
#include "text.h"
#include "webrequest.h"
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define START_FRAME 0
#define AP_ANIM_EVERY_FRAME 45

void animate_wifi_ap();
void animate_sony_logi(bool fade_out);
static bool shutdown = false;

static const char *logoanim[] = {
    "sonylogo_00001.bmp", "sonylogo_00002.bmp", "sonylogo_00003.bmp", "sonylogo_00004.bmp", "sonylogo_00005.bmp",
    "sonylogo_00006.bmp", "sonylogo_00007.bmp", "sonylogo_00008.bmp", "sonylogo_00009.bmp", "sonylogo_00010.bmp",
    "sonylogo_00011.bmp", "sonylogo_00012.bmp", "sonylogo_00013.bmp", "sonylogo_00014.bmp", "sonylogo_00015.bmp",
    "sonylogo_00016.bmp", "sonylogo_00017.bmp", "sonylogo_00018.bmp", "sonylogo_00019.bmp", "sonylogo_00020.bmp",
    "sonylogo_00021.bmp", "sonylogo_00022.bmp", "sonylogo_00023.bmp", "sonylogo_00024.bmp", "sonylogo_00025.bmp",
    "sonylogo_00026.bmp", "sonylogo_00027.bmp", "sonylogo_00028.bmp", "sonylogo_00029.bmp", "sonylogo_00030.bmp",
    "sonylogo_00031.bmp", "sonylogo_00032.bmp", "sonylogo_00033.bmp", "sonylogo_00034.bmp", "sonylogo_00035.bmp",
    "sonylogo_00036.bmp", "sonylogo_00037.bmp", "sonylogo_00038.bmp", "sonylogo_00039.bmp", "sonylogo_00040.bmp",
    "sonylogo_00041.bmp", "sonylogo_00042.bmp", "sonylogo_00043.bmp", "sonylogo_00044.bmp", "sonylogo_00045.bmp",
    "sonylogo_00046.bmp", "sonylogo_00047.bmp", "sonylogo_00048.bmp", "sonylogo_00049.bmp", "sonylogo_00050.bmp",
    "sonylogo_00051.bmp", "sonylogo_00052.bmp", "sonylogo_00053.bmp", "sonylogo_00054.bmp", "sonylogo_00055.bmp",
    "sonylogo_00056.bmp", "sonylogo_00057.bmp", "sonylogo_00058.bmp", "sonylogo_00059.bmp", "sonylogo_00060.bmp",
    "sonylogo_00061.bmp", "sonylogo_00062.bmp", "sonylogo_00063.bmp", "sonylogo_00064.bmp", "sonylogo_00065.bmp",
    "sonylogo_00066.bmp", "sonylogo_00067.bmp", "sonylogo_00068.bmp", "sonylogo_00069.bmp", "sonylogo_00070.bmp",
    "sonylogo_00071.bmp", "sonylogo_00072.bmp", "sonylogo_00073.bmp", "sonylogo_00074.bmp", "sonylogo_00075.bmp",
    "sonylogo_00076.bmp", "sonylogo_00077.bmp", "sonylogo_00078.bmp", "sonylogo_00079.bmp", "sonylogo_00080.bmp",
    "sonylogo_00081.bmp", "sonylogo_00082.bmp", "sonylogo_00083.bmp", "sonylogo_00084.bmp", "sonylogo_00085.bmp",
    "sonylogo_00086.bmp", "sonylogo_00087.bmp", "sonylogo_00088.bmp", "sonylogo_00089.bmp", "sonylogo_00090.bmp",
    "sonylogo_00091.bmp", "sonylogo_00092.bmp", "sonylogo_00093.bmp", "sonylogo_00094.bmp", "sonylogo_00095.bmp",
    "sonylogo_00096.bmp", "sonylogo_00097.bmp", "sonylogo_00098.bmp", "sonylogo_00099.bmp", "sonylogo_00100.bmp",
    "sonylogo_00101.bmp", "sonylogo_00102.bmp", "sonylogo_00103.bmp", "sonylogo_00104.bmp", "sonylogo_00105.bmp",
    "sonylogo_00106.bmp", "sonylogo_00107.bmp", "sonylogo_00108.bmp", "sonylogo_00109.bmp", "sonylogo_00110.bmp",
    "sonylogo_00111.bmp", "sonylogo_00112.bmp", "sonylogo_00113.bmp", "sonylogo_00114.bmp", "sonylogo_00115.bmp",
    "sonylogo_00116.bmp", "sonylogo_00117.bmp", "sonylogo_00118.bmp", "sonylogo_00119.bmp", "sonylogo_00120.bmp",
    "sonylogo_00121.bmp", "sonylogo_00122.bmp", "sonylogo_00123.bmp", "sonylogo_00124.bmp", "sonylogo_00125.bmp",
    "sonylogo_00126.bmp", "sonylogo_00127.bmp", "sonylogo_00128.bmp", "sonylogo_00129.bmp", "sonylogo_00130.bmp",
    "sonylogo_00131.bmp", "sonylogo_00132.bmp", "sonylogo_00133.bmp", "sonylogo_00134.bmp", "sonylogo_00135.bmp",
    "sonylogo_00136.bmp", "sonylogo_00137.bmp", "sonylogo_00138.bmp", "sonylogo_00139.bmp", "sonylogo_00140.bmp",
    "sonylogo_00141.bmp", "sonylogo_00142.bmp", "sonylogo_00143.bmp", "sonylogo_00144.bmp", "sonylogo_00145.bmp",
    "sonylogo_00146.bmp", "sonylogo_00147.bmp", "sonylogo_00148.bmp", "sonylogo_00149.bmp", "sonylogo_00150.bmp",
    "sonylogo_00151.bmp", "sonylogo_00152.bmp", "sonylogo_00153.bmp", "sonylogo_00154.bmp", "sonylogo_00155.bmp",
    "sonylogo_00156.bmp", "sonylogo_00157.bmp", "sonylogo_00158.bmp", "sonylogo_00159.bmp",
};

static gimp_bitmap_t *sony_bm[sizeof(logoanim) / sizeof(logoanim[0])];

void intShutdown(int sign) {
    (void)sign;
    shutdown = true;
}

typedef enum {
    EVENT_LEFT_PRESS_SHORT,
    EVENT_LEFT_PRESS_LONG,
    EVENT_RIGHT_PRESS_SHORT,
    EVENT_RIGHT_PRESS_LONG,
    EVENT_N,
} event_key_handle_e;

#define EVENT_TEXT_SIZE 50
void key_press_callback(double presstime, key_e key, int handle) {

    char testtext[EVENT_TEXT_SIZE] = "";

    (void)presstime;
    (void)key;

    container_reset(9);
    switch (handle) {
        case EVENT_LEFT_PRESS_SHORT:
            snprintf(testtext, EVENT_TEXT_SIZE, "Left\nShort\n%0.2fs", presstime);
            break;
        case EVENT_LEFT_PRESS_LONG:
            snprintf(testtext, EVENT_TEXT_SIZE, "Left\nLong\n%0.2fs", presstime);

            break;
        case EVENT_RIGHT_PRESS_SHORT:
            snprintf(testtext, EVENT_TEXT_SIZE, "Right\nShort\n%0.2fs", presstime);

            break;
        case EVENT_RIGHT_PRESS_LONG:
            snprintf(testtext, EVENT_TEXT_SIZE, "Right\nLong\n%0.2fs", presstime);

            break;
        default:
            break;
    }
    text_put(KEY_INFO, testtext);
}

int main() {

    gimp_bitmap_t *bitmap;

    /* Catch a whole bunch of signals and shutdown properly */
    signal(SIGINT, intShutdown);
    signal(SIGTERM, intShutdown);
    signal(SIGSTOP, intShutdown);
    signal(SIGKILL, intShutdown);

    LOG("Git sha: %s\nBuild date: %s\n\n", GITSHA, DATE);

    /* Initialize the framebuffer and container lib */
    container_new(&containers);

    /* Initialize the text/font lib */
    text_init();

    keys_register_cb(key_press_callback, KEY_CHAR_RIGHT, 1, EVENT_RIGHT_PRESS_LONG);
    keys_register_cb(key_press_callback, KEY_CHAR_LEFT, 1, EVENT_LEFT_PRESS_LONG);

    keys_register_cb(key_press_callback, KEY_CHAR_LEFT, 0.1, EVENT_LEFT_PRESS_SHORT);
    keys_register_cb(key_press_callback, KEY_CHAR_RIGHT, 0.1, EVENT_RIGHT_PRESS_SHORT);

    if (keys_start()) {
        goto on_err;
    }

    if (webrequest_init()) {
        goto on_err;
    }

    if (fwatch_start()) {
        goto on_err;
    }

    container_visible(SONY_LOGO, true);
    container_visible(AP_A, true);
    container_visible(AP_B, false);
    container_visible(AP_C, false);
    container_visible(AP_D, false);

    if (bmp_open_path("graphics", "wifi_ap_a.bmp", &bitmap) != 0)
        return -1;
    container_fill_bitmap(AP_A, bitmap, true);
    bmp_free(&bitmap);

    if (bmp_open_path("graphics", "wifi_ap_b.bmp", &bitmap) != 0)
        return -1;
    container_fill_bitmap(AP_B, bitmap, true);
    bmp_free(&bitmap);

    if (bmp_open_path("graphics", "wifi_ap_c.bmp", &bitmap) != 0)
        return -1;
    container_fill_bitmap(AP_C, bitmap, true);
    bmp_free(&bitmap);

    if (bmp_open_path("graphics", "wifi_ap_d.bmp", &bitmap) != 0)
        return -1;
    container_fill_bitmap(AP_D, bitmap, true);
    bmp_free(&bitmap);

    text_put(WIFI_TEXTBOX, "Connect to WiFi SSID: PiAIZU\nGoto: http://192.168.1.1/");

    container_visible(WIFI_TEXTBOX, true);
    container_compile();

    container_fill_bitmap(AP_D, bitmap, true);
    bmp_free(&bitmap);

    text_put(TEXT, "AP");
    container_compile();
    for (unsigned int anim = START_FRAME; anim < sizeof(logoanim) / sizeof(logoanim[0]); anim++) {
        bmp_open_path("graphics/anim", logoanim[anim], &sony_bm[anim]);
    }

    animate_sony_logi(true);

    bool do_anim = true;
    do {

        do_anim = !do_anim;
        animate_sony_logi(do_anim);
        fwatch_do(25000);

    } while (!shutdown);
    LOG("Cleaning up and shutting down...\n");

on_err:
    for (unsigned int anim = START_FRAME; anim < sizeof(logoanim) / sizeof(logoanim[0]); anim++) {
        bmp_free(&sony_bm[anim]);
    }

    webrequest_close();
    container_close();
    text_close();

    return 0;
}
static int wifi = AP_B;
void       animate_wifi_ap() {

    do {
        if (wifi <= AP_D) {
            container_visible(wifi, true);

            wifi++;
        } else {
            container_visible(AP_A, true);
            container_visible(AP_B, false);
            container_visible(AP_C, false);
            container_visible(AP_D, false);

            wifi = AP_B;
        }
    } while (0);
}

void animate_sony_logi(bool fade_out) {
    unsigned int anim;

    if (fade_out) {
        for (anim = START_FRAME; anim < sizeof(logoanim) / sizeof(logoanim[0]) && !shutdown; anim++) {

            container_fill_bitmap(SONY_LOGO, sony_bm[anim], true);

            if (anim % AP_ANIM_EVERY_FRAME == 0) {
                animate_wifi_ap();
            }
            container_compile();
            fwatch_do(4000);
        }
    } else {
        for (anim = (sizeof(logoanim) / sizeof(logoanim[0])) - 1; anim > START_FRAME && !shutdown; anim--) {

            container_fill_bitmap(SONY_LOGO, sony_bm[anim], true);

            if (anim % AP_ANIM_EVERY_FRAME == 0) {
                animate_wifi_ap();
            }
            container_compile();
            fwatch_do(4000);
        }
    }
}
