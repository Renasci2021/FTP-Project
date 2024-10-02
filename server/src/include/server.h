#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include "session.h"

int create_server_socket(int port);
int create_passive_socket();
int create_data_connection(ClientSession *session);
void close_data_connection(ClientSession *session);
void *handle_client(void *arg);

#endif // SERVER_H
