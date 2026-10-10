#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include "network.h"

void handle_connection(int client_fd) {
    FILE *index_html;
    index_html = fopen("index.html", "r");

    if (index_html == NULL) {
        perror("error opening index.html");
        quick_exit(3);
    }

    // go to the end of the file
    fseek(index_html, 0L, SEEK_END);

    // check the file size
    long file_size = ftell(index_html);

    // go back to beginning of file
    rewind(index_html);

    printf("file is: %ld bytes long\n", file_size);
    char *html_header;
    asprintf(&html_header,
    "HTTP/1.1 200 OK\r\n"
    "Server: callu-ws\r\n"
    "Accept-Ranges: bytes\r\n"
    "Content-Length: %ld\r\n"
    "Content-Type: text/html\r\n"
    "\r\n",
    file_size);

    write(client_fd, html_header, strlen(html_header));

    // write in the file whilst handling partial writes

    char buffer[4096] = "";
    size_t read_bytes = 0;
    while ((read_bytes = fread(buffer, 1, sizeof(buffer), index_html)) > 0) {
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
