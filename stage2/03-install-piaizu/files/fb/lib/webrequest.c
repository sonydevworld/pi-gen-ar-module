#include "fwatch.h"
#include "lmxsocket.h"
#include "log.h"
#include "text.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

static int          sock_fd; /* Just to be used when closing the file */
static coordinate_t current_coords = {.x = LMX_PIXEL_WIDTH / 2, .y = LMX_PIXEL_HEIGHT / 2};

void webrequest_update_text(unsigned int c, char *text) {
    container_reset(c);
    text_put(c, text);
}

void webrequest_update_crosshair(unsigned int c, coordinate_t *coord) {
    container_reset(c);

    container_draw_line(c, 0, coord->y, LMX_PIXEL_WIDTH - 1, coord->y); /* Horisontal line */
    container_draw_line(c, coord->x, 0, coord->x, LMX_PIXEL_HEIGHT - 1); /* Vertical line */
    current_coords.x = coord->x;
    current_coords.y = coord->y;
}

void webrequest_handle(int sock) {
    ssize_t                   readsize;
    struct sockaddr_un        src_addr;
    socklen_t                 src_addr_len;
    char                      buf[DATAGRAM_SIZE];
    socket_datagram_header_t *sdh;

    src_addr_len = sizeof(struct sockaddr_un);
    readsize     = recvfrom(sock, buf, DATAGRAM_SIZE, 0, (struct sockaddr *)&src_addr, &src_addr_len);

    if (readsize < 0) {
        LOG("Receiving datagram packet %d %m", sock);
        exit(1);
    }

    sdh = (void *)buf;

    switch (sdh->type) {
        case SOCKET_DATAGRAM_TYPE_COORD: {
            socket_datagram_lmx_coordinate_t *coord = (socket_datagram_lmx_coordinate_t *)sdh;
            LOG("Received coordinate: %zd bytes  \n=======\nx:%d y:%d\n=======\n", readsize, coord->coord.x,
                coord->coord.y);
            webrequest_update_crosshair(11, &coord->coord);
            break;
        }
        case SOCKET_DATAGRAM_TYPE_MOVE: {
            socket_datagram_lmx_move_t *coord = (socket_datagram_lmx_move_t *)sdh;
            coordinate_t                new_coord;
            new_coord.x = current_coords.x;
            new_coord.y = current_coords.y;

            switch (coord->move) {
                case MOVE_UP:
                    if (new_coord.y) {
                        new_coord.y--;
                    }
                    break;
                case MOVE_DOWN:
                    new_coord.y++;
                    break;
                case MOVE_LEFT:
                    if (new_coord.x) {
                        new_coord.x--;
                    }
                    break;
                case MOVE_RIGHT:
                    new_coord.x++;
                    break;
                case MOVE_UP_LEFT:
                    if (new_coord.y) {
                        new_coord.y--;
                    }
                    if (new_coord.x) {
                        new_coord.x--;
                    }
                    break;
                case MOVE_UP_RIGHT:
                    if (new_coord.y) {
                        new_coord.y--;
                    }
                    new_coord.x++;
                    break;
                case MOVE_DOWN_LEFT:
                    if (new_coord.x) {
                        new_coord.x--;
                    }
                    new_coord.y++;
                    break;
                case MOVE_DOWN_RIGHT:
                    new_coord.x++;
                    new_coord.y++;
                    break;
                case MOVE_CLEAR:
                    container_reset(11);
                    return;
            }
            LOG("Received move: %zd bytes  \n=======\nx:%d y:%d Container: %d\n=======\n", readsize, new_coord.x,
                new_coord.y, coord->header.container);
            webrequest_update_crosshair(coord->header.container, &new_coord);
            break;
        }
        case SOCKET_DATAGRAM_TYPE_TEXT: {
            socket_datagram_lmx_text_t *text = (socket_datagram_lmx_text_t *)sdh;
            LOG("Received text: %zd bytes  \n=======\n%s\n=======\n", readsize, text->text);
            webrequest_update_text(10, text->text);
            break;
        }

        case SOCKET_DATAGRAM_TYPE_INFO: {
            int                         i, n;
            socket_datagram_lmx_info_t *response;
            container_t *               c;
            size_t                      s;

            n = container_get_n();
            s = sizeof(socket_datagram_lmx_info_t) + (n * sizeof(container_info_t));

            response = malloc(s);

            response->header.type  = SOCKET_DATAGRAM_TYPE_INFO;
            response->header.size  = s;
            response->n_containers = n;
            for (i = 0; i < n; i++) {
                container_info_t *ci = &response->containers[i];
                c                    = container_get(i);

                ci->x             = c->x;
                ci->y             = c->y;
                ci->width         = c->width;
                ci->height        = c->height;
                ci->transparent   = c->transparent;
                ci->visible       = c->visible;
                ci->frame         = c->frame;
                ci->framesize_mem = c->framesize_mem;
                strncpy(ci->description, c->description, DESCRIPTION_LENGHT);
            }

            LOG("Received info request: %zd bytes, sending response %zd bytes to "
                "src_addr %s\n",
                readsize, s, src_addr.sun_path);
            if (sendto(sock, response, s, 0, (struct sockaddr *)&src_addr, src_addr_len) < 0) {
                LOG("Error sending datagram message: %m\n");
            }
            free(response);
            break;
        }
        default: {
            LOG("Received: %zd bytes but command is not supported: %d\n", readsize, sdh->type);
            break;
        }
    }
    return;
}

int webrequest_init() {

    int                sock;
    struct sockaddr_un name;

    /* Create socket from which to read. */
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("opening datagram socket");
        return -1;
    }

    /* Create name. */
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path));

    /* Bind the UNIX domain address to the created socket */
    if (bind(sock, (struct sockaddr *)&name, sizeof(struct sockaddr_un))) {
        perror("binding name to datagram socket");
        return -1;
    }
    chmod(SOCKET_NAME, 0777);

    fwatch_add(webrequest_handle, sock, SOCKET_NAME);
    sock_fd = sock;
    return 0;
}

void webrequest_close() {
    close(sock_fd);
    unlink(SOCKET_NAME);
}
