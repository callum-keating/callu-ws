#pragma once

#include <stdint.h>
void handle_connection(int client_fd);

enum methods {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
};

#define HTTP_ERROR_NONE 0           // success
#define HTTP_ERROR_MALFORMED -1     // if http header is malformed
#define HTTP_ERROR_NONEXISTENT -2   // if requested page does not exist


struct http_req {
    uint8_t version; // 1 for 1.1, 2 for 2, and 3 for 3
    char *path;      // path (not including www/)
    char *host;      // e.g. dogs.com
    enum methods method;
};
