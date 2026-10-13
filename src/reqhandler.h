#include <stdint.h>
void handle_connection(int client_fd);

enum methods {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
};

struct http_req {
    uint8_t version; // 1 for 1.1, 2 for 2, and 3 for 3
    char *path;      // path (not including www/)
    char *host;      // e.g. dogs.com
    enum methods method;
};
