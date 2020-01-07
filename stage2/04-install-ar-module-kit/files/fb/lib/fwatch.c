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

#define max(a, b)                                                                                                      \
    ({                                                                                                                 \
        __typeof__(a) _a = (a);                                                                                        \
        __typeof__(b) _b = (b);                                                                                        \
        _a > _b ? _a : _b;                                                                                             \
    })

#define FWATCH_CLIENTS_MAX 5

typedef struct {
    fwatch_cb_f cb;
    int         fd;
} fwatch_client_t;

typedef struct {
    unsigned int    n_registered;
    fd_set          rfds;
    struct timeval  timeout;
    int             maxfd;
    fwatch_client_t clients[FWATCH_CLIENTS_MAX];
} fwatch_t;

static fwatch_t fwatch;

int fwatch_add(fwatch_cb_f cb, int fd, char *fname) {

    if (!cb) {
        return -1;
    }
    if (fwatch.n_registered >= FWATCH_CLIENTS_MAX) {
        return -1;
    }
    if (fd == -1) {
        fd = open(fname, O_RDONLY);
        if (fd == -1) {
            LOG("Registered %s fd:%d callback %p\n", fname, fd, cb);
            return -1;
        }
    }

    LOG("Registered %s fd:%d callback %p\n", fname, fd, cb);
    fwatch.clients[fwatch.n_registered].cb = cb;
    fwatch.clients[fwatch.n_registered].fd = fd;

    fwatch.n_registered++;

    return 0;
}

int fwatch_start() {

    unsigned int i;
    int          maxfd = 0;

    FD_ZERO(&fwatch.rfds);

    for (i = 0; i < fwatch.n_registered; i++) {
        int fd = fwatch.clients[i].fd;
        maxfd  = max(maxfd, fd);
        FD_SET(fd, &fwatch.rfds);
    }
    fwatch.maxfd = maxfd;
    return 0;
}

int fwatch_do(int us) {

    int            retval;
    struct timeval timeout;
    unsigned int   i;

    timeout.tv_sec  = us / 1000000;
    timeout.tv_usec = us % 1000000;

    for (i = 0; i < fwatch.n_registered; i++) {
        int fd = fwatch.clients[i].fd;
        FD_SET(fd, &fwatch.rfds);
    }

    retval = select(fwatch.maxfd + 1, &fwatch.rfds, NULL, NULL, &timeout);

    if (retval == -1) {
        LOG("Error occured while waiting for select %m\n");
        return -1;
    }

    if (retval == 0) {
        return 0;
    }

    if (retval > 0) {

        for (i = 0; i < fwatch.n_registered; i++) {

            int fd = fwatch.clients[i].fd;

            if (FD_ISSET(fd, &fwatch.rfds)) {

                if (fwatch.clients[i].cb) {
                    fwatch.clients[i].cb(fd);
                }
            }
        }
    }
    return 0;
}
