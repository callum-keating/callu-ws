#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>


int main() {
    printf("Hello, Webserver!\n");

    struct sockaddr_in my_addr = {
        .sin_family = AF_INET,          // use ipv4
        .sin_port = htons(1234),        // use port 1234. htons converts to network byte
        .sin_addr.s_addr = INADDR_ANY,  // accepts connections from any local ipv4 address
    };


    // create a socket file descriptor with ipv4, tpc, and no protocol
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) {
        perror("error when creating socket");
        return 1;
    }

    int opt = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(socketfd);
        return 1;
    }

    // bind to port and address
    if (bind(socketfd, (const struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
        perror("error when binding");
        return 2;
    }

    listen(socketfd, 10);
    int viscount = 0;
    while (1) {
        struct sockaddr_in client_addr = {0};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("error when accepting request");
            return 3;
        }
        printf("CLIENT CONNECTED!\n");
        char message[50];
        snprintf(message, sizeof(message), "%s%d%s", "you are vistor number ", ++viscount, "\n");
        write(client_fd, message, sizeof(message));
        close(client_fd);
    }

    close(socketfd);

    printf("ran successfully\n");
    return 0;
}
