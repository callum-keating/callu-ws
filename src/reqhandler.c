#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void handle_connection(int client_fd) {
    char read_bytes[4096] = "";
    ssize_t bytes_read = read(client_fd, read_bytes, sizeof(read_bytes));
    size_t index = 0;

    // extract the first line
    while (read_bytes[index] != '\n' && read_bytes[index] != '\0') {
        index++;
        printf("%zu\n", index);
    }

    printf("%zu\n", index);
    char new_buffer[4096] = "";
    strncpy(new_buffer, read_bytes, index);
    printf("%s\n", new_buffer);

    size_t token_count = 0;
    // parse the first line
    char *token = strtok(new_buffer, " ");
    char *tokens[3];
    // split into tokens
    while (token != NULL) {
        token_count++;
        if (token_count > 3) {
            // MALFORMED
            // break for now
            break;
        }
        printf("Token: %s\n", token);
        tokens[token_count - 1] = token;
        token = strtok(NULL, " ");
    }

    printf("Method: %s\n", tokens[0]);
    printf("Path: %s\n", tokens[1]);
    printf("Version: %s\n", tokens[2]);


    // gather the methods that are important

    // return the response
}