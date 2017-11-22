#include "fcgi_stdio.h"
#include "lmxsocket.h"
#include <jansson.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static char *nfo[] = {
    "QUERY_STRING", "REQUEST_METHOD", "CONTENT_TYPE", "CONTENT_LENGTH", "SCRIPT_FILENAME", "SCRIPT_NAME", "PATH_INFO",
    "PATH_TRANSLATED", "REQUEST_URI", "DOCUMENT_URI", "DOCUMENT_ROOT", "SERVER_PROTOCOL", "GATEWAY_INTERFACE",
    "SERVER_SOFTWARE", "REMOTE_ADDR", "REMOTE_PORT", "SERVER_ADDR", "SERVER_PORT", "SERVER_NAME", "HTTPS",
};
#if DBG

#define HTML_COMMENT HTML_PRE
#define HTML_COMMENT_END HTML_PRE_END


#else

#define HTML_COMMENT "<!--"
#define HTML_COMMENT_END "-->"

#endif

#define HTML_PRE "<PRE>"
#define HTML_PRE_END "</PRE>"


#define ARRAY_SIZE(x) sizeof(x) / sizeof(x[0])

#define FOR_EACH_WEB_REQUEST_STR(x) "LMX_POST_" #x,

static char *webrequest_cmd[] = {WEBREQUEST(FOR_EACH_WEB_REQUEST_STR)};
static int   sock;

int send_datagram(socket_datagram_header_t *data, void **response_data) {

    struct sockaddr_un name;
    struct sockaddr_un my_name;

    if (!data) {
        return -1;
    }

    if (!data->size) {
        return -1;
    }

    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("<P><B>Error opening datagram socket</B>");
        return -1;
    }

    /* Server name */
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path));

    /* Client name*/
    memset(&my_name, 0, sizeof(struct sockaddr_un));
    my_name.sun_family = AF_UNIX;
    snprintf(my_name.sun_path, sizeof(my_name.sun_path), "/tmp/lmx_cgi.socket.%ld", (long)getpid());

    if (bind(sock, (struct sockaddr *)&my_name, sizeof(struct sockaddr_un)) == -1) {
        printf("error binding socket: %s\n", my_name.sun_path);
    }

    chmod(my_name.sun_path, 0777);

    /* Send message. */
    printf(HTML_COMMENT "Sending %d byte datagram via socket %s to the lmx-001 sample "
                        "application.." HTML_COMMENT_END,
        data->size, name.sun_path);

    if (sendto(sock, data, data->size, 0, &name, sizeof(struct sockaddr_un)) < 0) {
        printf("<P><B>Error sending datagram message</B>");
        close(sock);
        return -1;
    }

    if (response_data != NULL) {
        int  readsize;
        char receive_buf[DATAGRAM_SIZE];
        readsize = recvfrom(sock, receive_buf, DATAGRAM_SIZE, 0, NULL, NULL);
        if (readsize > 0) {
            *response_data = malloc(readsize);
            memcpy(*response_data, receive_buf, readsize);
        }
    }

    close(sock);
    unlink(my_name.sun_path);
    return 0;
}

int handle_request_get() {
    socket_datagram_lmx_info_t  lmx_info;
    socket_datagram_lmx_info_t *response = NULL;

    int i;
    int ret = 0;

    lmx_info.header.type = SOCKET_DATAGRAM_TYPE_INFO;
    lmx_info.header.size = sizeof(socket_datagram_lmx_info_t);

    ret = send_datagram((socket_datagram_header_t *)&lmx_info, (void **)&response);
    if (ret != 0) {
        printf("<P>Error sending datagram socket, lmx sample app most probably not "
               "started\n");
        return -1;
    }
    if (response == NULL) {
        printf("<P>Response is NULL\n");
        return -1;
    }
    printf("<P>Current container setup in the LMX sample application <PRE>\n");

    printf("Index, "
           "offset x, "
           "offset y, "
           "width, "
           "height,  "
           "transparent,"
           "visible, "
           "frame,  "
           "framesize_mem,"
           "Description\n========================================================================================\n");

    for (i = 0; i < response->n_containers; i++) {
        container_info_t *ci = &response->containers[i];

        printf("%02d "
               "      %3d "
               "      %3d "
               "     %3d "
               "     %3d "
               "   %d "
               "           %d "
               "        %d "
               "   %8d"
               "    %s\n",
            i, ci->x, ci->y, ci->width, ci->height, ci->transparent, ci->visible, ci->frame, ci->framesize_mem,
            ci->description);
    }
    printf(" </PRE>\n");
    free(response);
    return ret;
}

int handle_request_post() {

    int postlen = atoi(getenv("CONTENT_LENGTH"));
    socket_datagram_type_t i;
    int container = 0;
    if (postlen > 0) {

        char  buf[1024];
        char *cmd;
        char *data;
        int   len = postlen > 1024 ? 1024 - 1 : postlen;
        FCGI_fread(buf, len, 1, FCGI_stdin);
        printf(HTML_COMMENT "Raw post data:\n=================\n%s\n=================\n" HTML_COMMENT_END, buf);
        for (i = 0; i < SOCKET_DATAGRAM_TYPE_LAST; i++) {
            cmd = strstr(buf, webrequest_cmd[i]);
            printf(HTML_COMMENT "Testing cmd %s against \n{%s}\nResult: %s" HTML_COMMENT_END, webrequest_cmd[i],buf, cmd);
            if (cmd == NULL) {
                continue;
            } else {
                data = cmd + strlen(webrequest_cmd[i]) + 1;
                printf(HTML_COMMENT "Found command: <B>%.*s</B> and data: " HTML_COMMENT_END,
                    strlen(webrequest_cmd[i]), cmd, data);
                switch (i) {
                    case SOCKET_DATAGRAM_TYPE_CONTAINER:
                        container = atoi(data);
                        printf(HTML_COMMENT "Will use container: %d\n" HTML_COMMENT_END, container);
                        break;
                case SOCKET_DATAGRAM_TYPE_MOVE: {
                    movment_e    x;
                    static char *web_move_sub_cmd[] = {"MOVE_UP", "MOVE_DOWN", "MOVE_LEFT", "MOVE_RIGHT",
                        "MOVE_UP_LEFT", "MOVE_UP_RIGHT", "MOVE_DOWN_LEFT", "MOVE_DOWN_RIGHT", "MOVE_CLEAR"};

                    for (x = 0; x < sizeof(web_move_sub_cmd) / sizeof(web_move_sub_cmd[0]); x++) {
                        int y;
                        y   = strcmp(data, web_move_sub_cmd[x]);
                        if (y != 0) {
                            continue;
                        } else {
                            socket_datagram_lmx_move_t req;
                            req.header.type = SOCKET_DATAGRAM_TYPE_MOVE;
                            req.header.size = sizeof(socket_datagram_lmx_move_t);
                            req.header.container = container;
                            req.move        = x;
                            printf(HTML_PRE "%s container %d" HTML_PRE_END,data, req.header.container);
                            printf(HTML_COMMENT "<P> Sending move :%s result: %d" HTML_COMMENT_END, data,
                                send_datagram((socket_datagram_header_t *)&req, NULL));
                        }
                    }

                    break;
                }
                case SOCKET_DATAGRAM_TYPE_COORD: {
                    socket_datagram_lmx_coordinate_t dgcoord;
                    json_t *                         jcoord;
                    json_t *                         c;
                    json_t *                         x;
                    json_t *                         y;

                    dgcoord.header.type = SOCKET_DATAGRAM_TYPE_COORD;
                    dgcoord.header.size = sizeof(socket_datagram_lmx_coordinate_t);

                    json_error_t error;
                    jcoord = json_loads(data, 0, &error);

                    if (!jcoord) {
                        printf("error: on line %d: %s\n", error.line, error.text);
                        return -1;
                    }

                    c = json_object_get(jcoord, "coord");
                    if (!json_is_object(c)) {
                        printf("error: on line %d: %s\n", error.line, error.text);
                        return -1;
                    }

                    x = json_object_get(c, "x");
                    if (!json_is_integer(x)) {
                        printf("error: on line %d: %s\n", error.line, error.text);
                        return -1;
                    }

                    y = json_object_get(c, "y");
                    if (!json_is_integer(y)) {
                        printf("error: on line %d: %s\n", error.line, error.text);
                        return -1;
                    }
                    dgcoord.coord.x = json_integer_value(x);
                    dgcoord.coord.y = json_integer_value(y);

                    printf(HTML_COMMENT "<P><B>Sending coordinates datagram %d  "
                                        "struct size %d bytes, stream len %d "
                                        "data</B><PRE> x:%d "
                                        "y:%d<PRE><P>" HTML_COMMENT_END,
                        send_datagram((socket_datagram_header_t *)&dgcoord, NULL), dgcoord.header.size, len,
                        dgcoord.coord.x, dgcoord.coord.y);
                    printf("<P><PRE>x:%d y:%d</PRE>", dgcoord.coord.x, dgcoord.coord.y);

                    json_decref(x);
                    json_decref(y);
                    json_decref(c);
                    json_decref(jcoord);
                }

                break;
                case SOCKET_DATAGRAM_TYPE_TEXT: {
                    socket_datagram_lmx_text_t dgdata;
                    dgdata.header.type = SOCKET_DATAGRAM_TYPE_TEXT;
                    dgdata.header.size = sizeof(socket_datagram_lmx_text_t);
                    strncpy((char *)&dgdata.text, data, TEXT_LENGTH);
                    dgdata.text[TEXT_LENGTH - 1] = '\0';
                    printf(HTML_COMMENT "\n<P><B>Sending text datagram %d data %d struct size %d "
                                        "bytes, stream len %d, data "
                                        "</B><PRE>%s</PRE><P>" HTML_COMMENT_END,
                        send_datagram((socket_datagram_header_t *)&dgdata, NULL), TEXT_LENGTH, dgdata.header.size, len,
                        dgdata.text);
                    printf("<P><PRE>%s</PRE>", dgdata.text);
                    break;
                }
                        case SOCKET_DATAGRAM_TYPE_INFO:
                        case SOCKET_DATAGRAM_TYPE_LAST:
                        default:
                        break;

                }
            }
        }
    }
    return 0;
}

int main(void) {

    int          count = 0;
    unsigned int i;

    /* Response loop. */
    while (FCGI_Accept() >= 0) {
        printf("Content-type: text/html\r\n"
               "\r\n"
               "<title>PiAIZU CGI</title>");

        char *method = getenv("REQUEST_METHOD");

        if (!strcmp(method, "POST")) {
            handle_request_post();
        } else if (!strcmp(method, "GET")) {
            handle_request_get();
        } else {

            printf("<P>No data posted<BR>");
        }
    }

    printf(HTML_COMMENT);
    for (i = 0; i < sizeof(nfo) / sizeof(nfo[0]); i++) {
        printf("%02d: %s:  %s"  "\n", ++count, nfo[i], getenv(nfo[i]));
    }
    printf(HTML_COMMENT_END);
    return 0;
}
