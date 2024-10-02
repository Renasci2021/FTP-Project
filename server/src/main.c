#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include "include/arg_parser.h"
#include "include/server.h"
#include "include/globals.h"
#include "include/utils.h"

int port = DEFAULT_PORT;
char root_path[PATH_MAX_LEN] = DEFAULT_ROOT;

int debug_mode = 0;

int main(int argc, char *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    int result = parse_arguments(argc, argv, &port, root_path);
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
            continue;
        }
        log_info("[%d] New connection established\n", new_socket);

        // 创建新线程处理客户端请求
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, (void *)&new_socket) < 0)
        {
            log_error("Error creating thread: %s\n", strerror(errno));
            close(new_socket);
        }
        pthread_detach(thread); // 分离线程
    }

    return 0;
}
