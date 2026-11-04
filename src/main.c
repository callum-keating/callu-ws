#include <netinet/in.h>
#include <regex.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "network.h"
#include "reqhandler.h"

int main() {
  printf("Hello, Webserver!\n");
  int viscount = 0;
  char *port_env = getenv("PORT");
  int port;

  if (!port_env) {
    port = 1234;
  } else {
    port = atoi(port_env);
    if (port == 0) {
      return 1;
    }
  }
  printf("launching on port: %d\n", port);

  struct sockaddr_in my_addr = {
      .sin_addr.s_addr =
          INADDR_ANY,        // accepts connections from any local ipv4 address
      .sin_family = AF_INET, // use ipv4
      .sin_port = htons(port), // use port 1234. htons converts to network byte
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
