#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include "network.h"

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
        char message[] = "hello\n";
        write(client_fd, message, strlen(message));
        close(client_fd);
    }
    close_network();
    printf("ran successfully\n");
    return 0;
}
