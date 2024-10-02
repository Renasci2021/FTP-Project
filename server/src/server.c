#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
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
    if (listen(server_fd, SOMAXCONN) < 0)
    {
        return -1;
    }

    return server_fd; // 返回服务器套接字
}

// 创建被动模式套接字
int create_passive_socket()
{
    int port, server_fd;
    int max_attempts = 10; // 最大尝试次数
    srand(time(NULL));

    while (max_attempts--)
    {
        port = rand() % (65535 - 20000) + 20000; // 生成 20000 到 65535 之间的随机端口号

        if ((server_fd = create_server_socket(port)) >= 0)
        {
            return server_fd;
        }
    }

    return -1;
}

// 创建数据连接
int create_data_connection(ClientSession *session)
{
    if (session->data_mode == PORT_MODE)
    {
        int data_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (data_socket < 0)
        {
            return -1;
        }

        struct sockaddr_in data_address;
        data_address.sin_family = AF_INET;
        data_address.sin_port = htons(session->port);
        data_address.sin_addr.s_addr = inet_addr(session->ip_address);

        if (connect(data_socket, (struct sockaddr *)&data_address, sizeof(data_address)) < 0)
        {
            close(data_socket);
            return -1;
        }
        session->data_socket = data_socket;
    }
    else if (session->data_mode == PASV_MODE)
    {
        int data_socket = accept(session->pasv_socket, NULL, NULL);
        if (data_socket < 0)
        {
            close(session->pasv_socket);
            session->pasv_socket = 0;
            return -1;
        }
        close(session->pasv_socket);
        session->pasv_socket = 0;
        session->data_socket = data_socket;
    }
    else
    {
        log_error("Wrong data connection state: %d\n", session->data_mode);
        session->data_mode = 0;
        return -1;
    }

    log_info("[%d] Data connection established: %d\n", session->control_socket, session->data_socket);
    return 0;
}

// 关闭数据连接
void close_data_connection(ClientSession *session)
{
    if (session->data_socket > 0)
    {
        close(session->data_socket);
        log_info("[%d] Data connection closed: %d\n", session->control_socket, session->data_socket);
        session->data_socket = 0;
    }
    session->data_mode = 0;
}

// 处理客户端请求
void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE] = {0};
    int bytes_read;

    // 添加会话
    ClientSession *session = add_session(client_socket);
    if (session == NULL)
    {
        send_message(client_socket, "421 Too many users logged in.\r\n");
        close(client_socket);
        return NULL;
    }

    // 发送欢迎消息
    send_message(client_socket, "220 Anonymous FTP server ready.\r\n");

    while (session->is_connected)
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
            log_info("[%d] Client closed connection\n", client_socket);
            break;
        }

        // 去除字符串末尾的换行符
        buffer[bytes_read] = '\0';
        trim_crlf(buffer);
        log_info("[%d] %s\n", client_socket, buffer);

        // 处理客户端命令
        process_command(buffer, client_socket);
    }

    // 移除会话和关闭连接
    if (session->pasv_socket > 0)
    {
        close(session->pasv_socket);
    }
    if (session->data_socket > 0)
    {
        close(session->data_socket);
    }
    remove_session(client_socket);
    close(client_socket);
    log_info("[%d] Client disconnected\n", client_socket);
    return NULL;
}
