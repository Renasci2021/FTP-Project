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