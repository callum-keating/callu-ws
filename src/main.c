#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

#include "network.h"

void handle_connection(int client_fd) {
    // read socket for file
    char read_buffer[4096] = "";
    read(client_fd, read_buffer, sizeof(read_buffer));
    printf("%s\n", read_buffer);

    //  A request message from a client to a server includes, within the first line of that message, the method to be applied to the resource, the identifier of the resource, and the protocol version in use.  meaning I can assume the first line WILL be GET /whatever HTTP/1.1
    if (strncmp(read_buffer, "GET", 3)) {
        printf("METHOD IS NOT GET\n");
        close(client_fd);
        return;
    }

    regex_t regex;
    if (regcomp(&regex, "GET /([^[:space:]]+)? HTTP/([0-9.]+)", REG_EXTENDED) != 0) {
        perror("Error compiling regex");
        close(client_fd);
        quick_exit(4);
    }
    
    regmatch_t matches[3];
    if (regexec(&regex, read_buffer, 3, matches, 0) == REG_NOMATCH) {
        printf("DID NOT MATCH REGEX\n");
        close(client_fd);
        return;
    }

    char *path;
    regmatch_t path_match = matches[1];
    if (path_match.rm_so < 0 || path_match.rm_eo < 0) {
        printf("PATH IS JUST / USING index.html\n");
        path = strdup("index.html");
    } else {
        size_t path_size = (size_t)path_match.rm_eo - path_match.rm_so;
        path = malloc(path_size + 1);
        memcpy(path, read_buffer + path_match.rm_so, path_size);
        path[path_size] = '\0';
    }

    printf("path: %s\t\t %lu %lu\n", path, strlen(path), strlen("www/"));

    size_t file_path_size = strlen("www/") + strlen(path) + 1;
    char *file = malloc(file_path_size);

    if (file == NULL) {
        perror("error when mallocing file name buffer");
        free(path);
        return;
    }
    strcat(file, path);
    snprintf(file, file_path_size, "www/%s", path);

    FILE *ret_file;
    ret_file = fopen(file, "r");

    if (ret_file == NULL) {
        perror("error opening index.html");
        close(client_fd);
        quick_exit(3);
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

int main() {
    printf("Hello, Webserver!\n");
    int viscount = 0;
    struct sockaddr_in my_addr = {
        .sin_addr.s_addr = INADDR_ANY,   // use ipv4
        .sin_family = AF_INET,           // use port 1234. htons converts to network byte
        .sin_port = htons(1234),         // accepts connections from any local ipv4 address
    };


    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    init_network(&my_addr, sockfd);

    while (1) {
        int client_fd = wait_until_connection();
        handle_connection(client_fd);
    }
    close_network();
    printf("ran successfully\n");
    return 0;
}
