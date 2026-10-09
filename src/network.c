#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

static struct sockaddr_in *my_addr = {0};


// create a socket file descriptor with ipv4, tpc, and no protocol
static int socketfd;

int init_network(struct sockaddr_in *addr_struct, int socket_filedesc) {
    my_addr = addr_struct;

    // create a socket file descriptor with ipv4, tpc, and no protocol
    socketfd = socket_filedesc;

    if (socketfd == -1) {
        perror("error when creating socket");
        quick_exit(1);
    }

    int opt = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(socketfd);
        quick_exit(1);
    }

    // bind to port and address
    if (bind(socketfd, (const struct sockaddr *)my_addr, sizeof(*my_addr)) == -1) {
        perror("error when binding");
        quick_exit(2);
    }

    listen(socketfd, 10);
    return 0;
}

void wait_until_connection(char *message) {
    struct sockaddr_in client_addr = {0};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("error when accepting request");
        quick_exit(3);
    }
    printf("CLIENT CONNECTED!\n");

    write(client_fd, message, strlen(message));
    close(client_fd);
}

void close_network() {
    close(socketfd);
}
