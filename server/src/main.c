#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "include/server.h"

#define PORT 21

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // 创建套接字
    server_fd = create_server_socket(PORT);
    if (server_fd < 0)
    {
        perror("Failed to create server socket");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("New connection established\n");

        // 处理客户端请求
        handle_client(new_socket);

        // 关闭连接
        close(new_socket);
    }

    return 0;
}
