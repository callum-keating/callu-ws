#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network.h"

int main() {
    printf("Hello, Webserver!\n");

    int viscount = 0;
    init_network();
    while (1) {
        wait_until_connection(++viscount);
    }
    close_network();
    printf("ran successfully\n");
    return 0;
}
