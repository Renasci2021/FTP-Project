#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include "include/arg_parser.h"
#include "include/server.h"
#include "include/globals.h"
#include "include/utils.h"

int debug_mode = 0;

int main(int argc, char *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int port;
    char *root;

    int result = parse_arguments(argc, argv, &port, &root);
    if (result <= 0)
    {
        return result;
    }

    // 创建套接字
    server_fd = create_server_socket(port);
    if (server_fd < 0)
    {
        log_error("Failed to create server socket");
        exit(EXIT_FAILURE);
    }

    log_info("Server is listening on port %d\n", port);

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            log_error("Error accepting connection: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        log_info("New connection established\n");

        // 处理客户端请求
        handle_client(new_socket);

        // 关闭连接
        close(new_socket);
    }

    return 0;
}
