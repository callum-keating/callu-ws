int init_network(struct sockaddr_in *addr_struct, int socket_filedesc);
int wait_until_connection();
void close_network();
