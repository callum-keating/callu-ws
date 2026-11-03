#include <unistd.h>
#include <stdio.h>

void handle_connection(int client_fd) {
    char buffer[4096] = "";
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer));
    printf("%s\n", buffer);
    // parse the first line
    
    // gather the methods that are important

    // return the response
}