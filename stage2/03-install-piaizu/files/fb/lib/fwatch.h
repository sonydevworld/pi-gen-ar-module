#pragma once

typedef void (*fwatch_cb_f)(int fd);

/**
 * @param us Timeout time for select in µS
 *
 * @returns 0 on success, -1 on error.
 */
int fwatch_do(int us);

/**
 *
 * @returns 0
 *
 *      Will start the module. No more registration should be done after start has been called.
 */
int fwatch_start();

/**
 * @param cb    callback to be called when file is ready
 * @param fd    file descriptor, if zero, fname will be used to open file
 * @param fname filename of the file to select on
 *
 * @returns 0 on success, -1 on error
 *
 *      This function will call function cb when select notifies that
 *      the file in fd has data to be read.
 */
int fwatch_add(fwatch_cb_f cb, int fd, char *fname);
