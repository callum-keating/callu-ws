#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

#include "reqhandler.h"

static int get_params(char *http_header, struct http_req *http_req_struct) {
    char *path = NULL;
    //  A request message from a client to a server includes, within the first line of that message, the method to be applied to the resource, the identifier of the resource, and the protocol version in use.  meaning I can assume the first line WILL be GET /whatever HTTP/1.1
    if (strncmp(http_header, "GET", 3)) {
        printf("METHOD IS NOT GET\n");
        goto error;
    }
    http_req_struct->method = HTTP_METHOD_GET;

    regex_t regex;
    if (regcomp(&regex, "GET /([^[:space:]]+)? HTTP/([0-9.]+)", REG_EXTENDED) != 0) {
        perror("Error compiling regex");
        goto error;
        quick_exit(4);
    }

    regmatch_t matches[3];
    if (regexec(&regex, http_header, 3, matches, 0) == REG_NOMATCH) {
        printf("DID NOT MATCH REGEX\n");
        goto error;
    }

    regmatch_t path_match = matches[1];
    if (path_match.rm_so < 0 || path_match.rm_eo < 0) {
        printf("PATH IS JUST / USING index.html\n");
        path = strdup("index.html");
    } else {
        size_t path_size = (size_t)path_match.rm_eo - path_match.rm_so;
        path = malloc(path_size + 1);
        memcpy(path, http_header + path_match.rm_so, path_size);
        path[path_size] = '\0';
    }

    printf("path: %s\t\t %lu %lu\n", path, strlen(path), strlen("www/"));

    size_t file_path_size = strlen("www/") + strlen(path) + 1;
    char *file = malloc(file_path_size);

    if (file == NULL) {
        perror("error when mallocing file name buffer");
        goto error;
    }

    strcat(file, path);
    snprintf(file, file_path_size, "www/%s", path);

    http_req_struct->path = file;

    return 0;
error:
    if (path != NULL) {
        free(path);
    }
    return -1;
};


void handle_connection(int client_fd) {
    // read socket for file
    char read_buffer[4096] = "";
    read(client_fd, read_buffer, sizeof(read_buffer));
    struct http_req http_req_struct = {0};
    if (get_params(read_buffer, &http_req_struct) < 0) {
        close(client_fd);
        return;
    }
    printf("%s\n", read_buffer);



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
