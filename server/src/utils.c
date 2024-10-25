#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "include/utils.h"
#include "include/globals.h"

void trim_crlf(char *str)
{
    int len = strlen(str);

    if (str[len - 1] == '\n')
        str[len - 1] = '\0';
    if (str[len - 2] == '\r')
        str[len - 2] = '\0';
}

void log_error(const char *format, ...)
{
    if (debug_mode)
    {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    }
}

void log_info(const char *format, ...)
{
    if (debug_mode)
    {
        va_list args;
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
    }
}

void send_message(int client_socket, const char *message)
{
    send(client_socket, message, strlen(message), 0);
}

int calculate_path(char *input, char *path)
{
    // 计算工作目录
    // 以 / 开头，输入从工作目录始的绝对路径
    // 解析其中的 .. 和 .
    // 返回 0 表示成功，-1 表示失败
    // 若 .. 超出根目录，返回 -1

    if (input[0] != '/')
    {
        log_error("Invalid path: %s\n", input);
        return -1;
    }

    char *token = strtok(input, "/");
    path[0] = '/';
    int path_len = 1;

    while (token != NULL)
    {
        if (strcmp(token, "..") == 0)
        {
            // 返回上一级目录
            if (path_len == 1)
            {
                // 已经是根目录
                return -1;
            }

            // 删除最后一个目录
            while (path_len > 0 && path[path_len - 1] != '/')
            {
                path_len--;
            }

            if (path_len > 0)
            {
                path_len--;
            }
        }
        else if (strcmp(token, ".") != 0)
        {
            // 添加目录
            int token_len = strlen(token);
            if (path_len + token_len + 1 >= PATH_MAX_LEN)
            {
                log_error("Path too long: %s\n", input);
                return -1;
            }

            if (path_len > 0 && path[path_len - 1] != '/')
            {
                path[path_len++] = '/';
            }

            strcpy(path + path_len, token);
            path_len += token_len;
        }

        token = strtok(NULL, "/");
    }

    if (path_len == 0)
    {
        path[0] = '/';
        path[1] = '\0';
    }
    else
    {
        path[path_len] = '\0';
    }

    return 0;
}
