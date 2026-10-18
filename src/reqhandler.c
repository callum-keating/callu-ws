#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include "reqhandler.h"

static void freeHeader(struct http_req *http_struct) {
    free(http_struct->path);
};

void handle_404(int client_fd) {
    char *http_response = NULL;
    printf("RETURNING 404\n");
    asprintf(&http_response,
    "HTTP/1.1 404 Not Found\r\n"
    "Server: callu-ws\r\n"
    "\r\n");

    write(client_fd, http_response, strlen(http_response));
    free(http_response);
};

static int get_params(char *http_header, struct http_req *http_struct) {
    int retVal = HTTP_ERROR_NONE;
    char *headerToTokenize = strdup(http_header);

    char *token = strtok(headerToTokenize, " ");
    if (token == NULL || strcmp(token, "GET") != 0)
        // for now i will only accept GET requests
        goto malformed;

    token = strtok(NULL, " ");

    // if the first character isn't a /
    if (strncmp(token, "/", 1)) {
        goto malformed;
    }
    http_struct->path = strdup(token);
    if (strcmp(http_struct->path, "/") == 0) {
        free(http_struct->path);
        http_struct->path = strdup("/index.html");
    }
    http_struct->method = HTTP_METHOD_GET;

    goto finish;
malformed:
    retVal = HTTP_ERROR_MALFORMED;
finish:
    free(headerToTokenize);
    return retVal;
};


void handle_connection(int client_fd) {
    char read_buffer[4096] = "";
    char *fpath = NULL;

    // read socket for file
    read(client_fd, read_buffer, sizeof(read_buffer));
    struct http_req http_req_struct = {0};

    if (get_params(read_buffer, &http_req_struct) != 0) {
        printf("ERROR IN GET_PARAMS\n");
        goto finish;
    }

    if (asprintf(&fpath, "www%s", http_req_struct.path) == -1) {
        printf("ERROR IN asprintf\n");
        goto finish;
    }

    printf("READ BUFFER: %s\n", read_buffer);
    printf("HTTP PATH: %s\n", http_req_struct.path);
    printf("FILE PATH: %s\n", fpath);



    FILE *ret_file;
    ret_file = fopen(fpath, "r");

    if (ret_file == NULL) {
        printf("error opening file %s\n", fpath);
        handle_404(client_fd);
        goto finish;
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

finish:
    free(fpath);
    freeHeader(&http_req_struct);
    close(client_fd);
}
