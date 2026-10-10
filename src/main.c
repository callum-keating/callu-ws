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

    // to fit the \r\n
    file_size+=2;

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

    // write in the file

    char buffer[4096] = "";
    while (fread(buffer, 1, sizeof(buffer), index_html) > 0) {
        write(client_fd, buffer, 4096);
    }

    write(client_fd, "\r\n", strlen("\r\n"));
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
