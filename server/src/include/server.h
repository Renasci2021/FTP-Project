#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>

int create_server_socket(int port);
int create_passive_socket();
void *handle_client(void *arg);

#endif // SERVER_H
