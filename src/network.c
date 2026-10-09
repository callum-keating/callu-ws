#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

static struct sockaddr_in my_addr = {0};


// create a socket file descriptor with ipv4, tpc, and no protocol
static int socketfd;

int init_network() {
    my_addr.sin_addr.s_addr = INADDR_ANY;   // use ipv4
    my_addr.sin_family = AF_INET;           // use port 1234. htons converts to network byte
    my_addr.sin_port = htons(1234);         // accepts connections from any local ipv4 address


    // create a socket file descriptor with ipv4, tpc, and no protocol
    socketfd = socket(AF_INET, SOCK_STREAM, 0);

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
    return 0;
}

void wait_until_connection(int viscount) {
    struct sockaddr_in client_addr = {0};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("error when accepting request");
        quick_exit(3);
    }
    printf("CLIENT CONNECTED!\n");

    char message[50] = "";
    snprintf(message, sizeof(message), "%s%d%s", "you are vistor number ", viscount, "\n");
    write(client_fd, message, sizeof(message));
    close(client_fd);
}

void close_network() {
    close(socketfd);
}
