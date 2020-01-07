#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#define SOCKET_NAME "/tmp/lmx.socket"
#define DATAGRAM_SIZE (10 * 1024)
#define TEXT_LENGTH 0x100
#define DESCRIPTION_LENGHT 256

#define WEBREQUEST(x) x(CONTAINER) x(COORD) x(TEXT) x(INFO) x(MOVE)

#define FOR_EACH_WEBREQUEST_ENUM(x) SOCKET_DATAGRAM_TYPE_##x,

typedef enum { WEBREQUEST(FOR_EACH_WEBREQUEST_ENUM) FOR_EACH_WEBREQUEST_ENUM(LAST) } socket_datagram_type_t;

typedef struct {
    socket_datagram_type_t type;
    int                    container;
    size_t                 size;
} socket_datagram_header_t;

typedef struct {
    uint16_t x;
    uint16_t y;
} coordinate_t;

typedef struct {
    socket_datagram_header_t header;
    coordinate_t             coord;
} socket_datagram_lmx_coordinate_t;

typedef enum {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP_LEFT,
    MOVE_UP_RIGHT,
    MOVE_DOWN_LEFT,
    MOVE_DOWN_RIGHT,
    MOVE_CLEAR,
} movment_e;

typedef struct {
    socket_datagram_header_t header;
    movment_e                move;
} socket_datagram_lmx_move_t;

typedef struct {
    socket_datagram_header_t header;
    char                     text[TEXT_LENGTH];
} socket_datagram_lmx_text_t;

typedef struct {
    int          x;
    int          y;
    unsigned int width;
    unsigned int height;
    bool         transparent;
    bool         visible;
    char         frame;
    size_t       framesize_mem;
    char         description[DESCRIPTION_LENGHT];
} container_info_t;

typedef struct {
    socket_datagram_header_t header;
    int                      n_containers;
    container_info_t         containers[];
} socket_datagram_lmx_info_t;
