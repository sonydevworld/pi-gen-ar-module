#pragma once

/**
 *
 *      Two buttons exist on the PiAIZU EXT shield.
 */
typedef enum {
    KEY_CHAR_LEFT = 105,
    KEY_CHAR_RIGHT,
} key_e;

/**
 *      The callback format to be used when registering for an
 *      event.
 */
typedef void (*key_press_f)(double downtime, key_e key, int handle);

/**
 * @param cb         Callback function pointer
 * @param key        Key to register, aka "button"
 * @param time       Time the key has to be held low before an event is
 * triggered
 * @param handle     passed as parameter into the callback function
 *
 *      This function must be called before start. After start has been called
 *      no new events can be registered.
 */
int keys_register_cb(key_press_f cb, key_e key, double time, int handle);

/**
 *
 *      Call this function to start the thread watching the low level events.
 */
int keys_start();

/**
 *      Clean up and close down thread
 */
int keys_close();
