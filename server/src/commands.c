#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "include/commands.h"

// 处理 FTP 命令
void process_command(char *command, int client_socket)
{
    // 示例命令处理
    if (strncmp(command, "USER", 4) == 0)
    {
        send(client_socket, "331 User name okay, need password\n", 36, 0);
    }
    else if (strncmp(command, "PASS", 4) == 0)
    {
        send(client_socket, "230 User logged in, proceed\n", 30, 0);
    }
    else if (strncmp(command, "QUIT", 4) == 0)
    {
        send(client_socket, "221 Service closing control connection\n", 40, 0);
        close(client_socket);
    }
    else
    {
        send(client_socket, "500 Unknown command\n", 20, 0);
    }
}
