#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int parse_requestl(char *requestln, char *parsed[3]) {
  size_t token_count = 0;
  // parse the first line
  char *token = strtok(requestln, " ");
  // split into tokens

  while (token != NULL) {

    token_count++;
    if (token_count > 3) {
      // MALFORMED
      // break for now
      break;
    }
    parsed[token_count - 1] = token;
    token = strtok(NULL, " ");
  }
  return 0;
}

void send_file(char *file_path) {
  FILE *retfile = fopen(file_path, "r");
  if (retfile == NULL) {
    // 404
    return;
  }
  return;
}

void handle_connection(int client_fd) {
  char read_bytes[4096] = "";
  ssize_t bytes_read = read(client_fd, read_bytes, sizeof(read_bytes));
  size_t index = 0;

  // extract the first line
  while (read_bytes[index] != '\n' && read_bytes[index] != '\0') {
    index++;
  }

  printf("%zu\n", index);
  char new_buffer[4096] = "";
  strncpy(new_buffer, read_bytes, index);
  printf("%s\n", new_buffer);

  char *tokens[3];
  parse_requestl(new_buffer, tokens);
  printf("Method: %s\n", tokens[0]);
  printf("Path: %s\n", tokens[1]);
  printf("Version: %s\n", tokens[2]);

  // return the response
  size_t path_size = strlen(tokens[1]) + 4;
  char *file_path = malloc(path_size);
  strncpy(file_path, "www", path_size);

  printf("before ncat\n");
  strncat(file_path, tokens[1], path_size);
  printf("the file is %s\n", file_path);

  send_file(file_path);
}