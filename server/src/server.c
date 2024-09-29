#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include "include/server.h"
#include "include/commands.h"
#include "include/session.h"
#include "include/utils.h"

// 创建服务器套接字
int create_server_socket(int port)
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        return -1; // 返回错误代码
    }

    // 允许端口重用
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        return -1;
    }

    // 设置地址结构
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // 绑定套接字
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        return -1;
    }

    // 开始监听连接
    if (listen(server_fd, 3) < 0)
    {
        return -1;
    }

    return server_fd; // 返回服务器套接字
}

// 处理客户端请求
void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE] = {0};
    int bytes_read;

    // 添加会话
    if (add_session(client_socket) < 0)
    {
        send_message(client_socket, "421 Too many users logged in.\r\n");
        close(client_socket);
        return NULL;
    }

    // 发送欢迎消息
    send_message(client_socket, "220 Anonymous FTP server ready.\r\n");

    while (1)
    {
        // 读取客户端发送的数据
        bytes_read = read(client_socket, buffer, BUFFER_SIZE);
        if (bytes_read < 0)
        {
            log_error("Error reading from socket: %s\n", strerror(errno));
            break;
        }
        else if (bytes_read == 0)
        {
            log_info("Client disconnected\n");
            break;
        }

        buffer[bytes_read] = '\0'; // 添加字符串结束符
        log_info("Received: %s", buffer);

        // 处理客户端命令
        process_command(buffer, client_socket);
    }

    // 移除会话和关闭连接
    remove_session(client_socket);
    close(client_socket);
    return NULL;
}
