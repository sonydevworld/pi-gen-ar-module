#include "keys.h"
#include "fwatch.h"
#include "log.h"
#include <fcntl.h>
#include <linux/input.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define N_KEY_PRESS_CALLBACKS 10
#define N_BUTTONS 2
#define WAIT_TIMEOUT_S 30
#define EVENT_FILE "/dev/input/event0"

/* internal typedefs */
typedef struct {
    key_e          key;
    struct timeval time_pressed;
    struct timeval time_released;
    double         time_hold;
} key_action_t;

typedef struct {
    key_press_f cb;
    key_e       key;
    double      time;
    int         handle;
} key_press_event_t;

static bool already_started = false;

static key_press_event_t client_callbacks[N_KEY_PRESS_CALLBACKS] = {{0}};

static unsigned int key_press_callback_index = 0;

int key_time_down(key_action_t *key) {

    struct timeval result;
    if (key == NULL) {
        return -1;
    }

    timersub(&key->time_released, &key->time_pressed, &result);
    key->time_hold = (double)result.tv_sec + ((double)result.tv_usec / 1000000);

    return 0;
}

static int keys_parse_callbacks(key_action_t *keyaction) {
    unsigned int i;
    if (!keyaction) {
        return -1;
    }

    for (i = 0; i < key_press_callback_index && i < N_KEY_PRESS_CALLBACKS; i++) {

        if (keyaction->key == client_callbacks[i].key) {

            if (keyaction->time_hold >= client_callbacks[i].time) {
                if (client_callbacks[i].cb != NULL) {
                    client_callbacks[i].cb(keyaction->time_hold, keyaction->key, client_callbacks[i].handle);

                    keyaction->time_hold = 0.0;
                    return 0;
                }
            }
        }
    }

    return 1;
}

int keys_register_cb(key_press_f cb, key_e key, double time, int handle) {

    if (!cb)
        return -1;

    if (already_started) {
        return -1;
    }

    if (key_press_callback_index < N_KEY_PRESS_CALLBACKS) {

        client_callbacks[key_press_callback_index].cb     = cb;
        client_callbacks[key_press_callback_index].key    = key;
        client_callbacks[key_press_callback_index].time   = time;
        client_callbacks[key_press_callback_index].handle = handle;

        key_press_callback_index++;
        return 0;
    }
    return -1;
}

void keys_file_watch_callback(int fd) {
    struct input_event evt;

    // clang-format off
    static key_action_t buttons[N_BUTTONS] = {
        {
            .key = KEY_CHAR_LEFT, .time_pressed = {0}, .time_released = {0}, .time_hold = 0,
        },
        {
            .key = KEY_CHAR_RIGHT, .time_pressed = {0}, .time_released = {0}, .time_hold = 0,
        }};
    // clang-format on

    read(fd, &evt, sizeof(struct input_event));
    if (evt.type == EV_SYN && evt.code == SYN_DROPPED) {

    } else if (evt.type == EV_KEY) {

        if (evt.value == 1) {

            switch (evt.code) {
                case KEY_CHAR_LEFT:
                case KEY_CHAR_RIGHT:
                    buttons[evt.code - KEY_CHAR_LEFT].time_pressed.tv_sec  = evt.time.tv_sec;
                    buttons[evt.code - KEY_CHAR_LEFT].time_pressed.tv_usec = evt.time.tv_usec;
                    buttons[evt.code - KEY_CHAR_LEFT].time_hold            = 0;

                    break;
                default:
                    // continue;
                    break;
            }

        } else {

            switch (evt.code) {
                case KEY_CHAR_LEFT:
                case KEY_CHAR_RIGHT:
                    buttons[evt.code - KEY_CHAR_LEFT].time_released.tv_sec  = evt.time.tv_sec;
                    buttons[evt.code - KEY_CHAR_LEFT].time_released.tv_usec = evt.time.tv_usec;
                    key_time_down(&buttons[evt.code - KEY_CHAR_LEFT]);
                    keys_parse_callbacks(&buttons[evt.code - KEY_CHAR_LEFT]);
                    break;
                default:
                    // continue;
                    break;
            }
        }
    }

    return;
}

void keys_sort_registered_callbacks() {
    unsigned int      a;
    unsigned int      b;
    key_press_event_t tmp;
    for (a = 0; a < key_press_callback_index; a++) {
        for (b = a + 1; b < key_press_callback_index; b++) {
            if (client_callbacks[a].time < client_callbacks[b].time) {
                memcpy(&tmp, &client_callbacks[b], sizeof(key_press_event_t));
                memcpy(&client_callbacks[b], &client_callbacks[a], sizeof(key_press_event_t));
                memcpy(&client_callbacks[a], &tmp, sizeof(key_press_event_t));
            }
        }
    }
}

int keys_start() {

    if (already_started) {

        return -1;
    }
    already_started = true;

    keys_sort_registered_callbacks();
    if (fwatch_add(keys_file_watch_callback, -1, EVENT_FILE) == 0) {
        return 0;
    }

    return 0;
}
