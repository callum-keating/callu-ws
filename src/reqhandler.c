#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include "reqhandler.h"

static int get_params(char *http_header, struct http_req *http_req_struct) {
    char *path = NULL;
    char *cpy = NULL;
    //  A request message from a client to a server includes, within the first line of that message, the method to be applied to the resource, the identifier of the resource, and the protocol version in use.  meaning I can assume the first line WILL be GET /whatever HTTP/1.1
    if (strncmp(http_header, "GET", 3)) {
        printf("METHOD IS NOT GET\n");
        goto error;
    }
    http_req_struct->method = HTTP_METHOD_GET;
    cpy = strdup(http_header);
    path = strchr(cpy, ' ');

    if (path == NULL)
        goto malformed;

    path++;

    if (*path != '/')
        goto malformed;

    path++;

    size_t until_space = 0;

    while (path[until_space] != ' ' && path[until_space] != '\0') {
        until_space++;
    }

    path[until_space] = '\0';

    char *directory = "www/";
    size_t len = strlen(directory);

    if (strcmp(path, "") == 0) {
        path = strdup("index.html");
    }

    // preappend the directory
    memmove(path + len, path, strlen(path) + 1);
    memcpy(path, directory, len);

    http_req_struct->path = strdup(path);

    free(cpy);
    return 0;
error:
    free(cpy);
    return -1;
malformed:
    free(cpy);
    return -2;
};


void handle_connection(int client_fd) {
    // read socket for file
    char read_buffer[4096] = "";
    read(client_fd, read_buffer, sizeof(read_buffer));
    struct http_req http_req_struct = {0};
    if (get_params(read_buffer, &http_req_struct) < 0) {
        free(http_req_struct.path);
        close(client_fd);
        return;
    }
    printf("READ BUFFER: %s\n", read_buffer);
    printf("FILE PATH: %s\n", http_req_struct.path);



    FILE *ret_file;
    ret_file = fopen(http_req_struct.path, "r");

    if (ret_file == NULL) {
        printf("error opening file %s\n", http_req_struct.path);
        free(http_req_struct.path);
        close(client_fd);
        return;
    }

    // go to the end of the file
    fseek(ret_file, 0L, SEEK_END);

    // check the file size
    long file_size = ftell(ret_file);

    // go back to beginning of file
    rewind(ret_file);

    printf("file is: %ld bytes long\n", file_size);
    char *http_header;
    asprintf(&http_header,
    "HTTP/1.1 200 OK\r\n"
    "Server: callu-ws\r\n"
    "Accept-Ranges: bytes\r\n"
    "Content-Length: %ld\r\n"
    "Content-Type: text/html\r\n"
    "\r\n",
    file_size);

    write(client_fd, http_header, strlen(http_header));


    // write in the file whilst handling partial writes
    char buffer[4096] = "";
    size_t read_bytes = 0;
    while ((read_bytes = fread(buffer, 1, sizeof(buffer), ret_file)) > 0) {
        size_t sent_bytes = 0;
        while (sent_bytes < read_bytes) {
            sent_bytes+=write(client_fd, buffer + sent_bytes, read_bytes - sent_bytes);
        }
    }

    close(client_fd);
}
