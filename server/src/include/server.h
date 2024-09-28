#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>

#define BUFFER_SIZE 1024 // 缓冲区大小

int create_server_socket(int port);
void handle_client(int client_socket);

#endif // SERVER_H
