#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "include/commands.h"
#include "include/utils.h"
#include "include/session.h"

void handle_user_command(char *command, int client_socket, ClientSession *session);
void handle_pass_command(char *command, int client_socket, ClientSession *session);

// 处理 FTP 命令
void process_command(char *command, int client_socket)
{
    ClientSession *session = find_session(client_socket);
    if (session == NULL)
    {
        log_error("Session not found for client socket %d\n", client_socket);
        return;
    }

    if (session->waiting_for_pass &&
        strncmp(command, "PASS", 4) != 0)
    {
        send_message(client_socket, "503 Bad sequence of commands.\r\n");
        return;
    }

    if (!session->logged_in &&
        strncmp(command, "USER", 4) != 0)
    {
        send_message(client_socket, "530 Please login with USER and PASS.\r\n");
        return;
    }

    // 保证一定是登录状态

    if (strncmp(command, "USER", 4) == 0)
    {
        if (session->logged_in)
        {
            send_message(client_socket, "530 Can't change user, user is already logged in.\r\n");
            return;
        }

        handle_user_command(command, client_socket, session);
    }
    else if (strncmp(command, "PASS", 4) == 0)
    {
        if (!session->waiting_for_pass)
        {
            send_message(client_socket, "503 Bad sequence of commands.\r\n");
            return;
        }

        handle_pass_command(command, client_socket, session);
    }
    else if (strncmp(command, "QUIT", 4) == 0)
    {
        send_message(client_socket, "221 Goodbye.\r\n");
    }
    else
    {
        send_message(client_socket, "500 Unknown command.\r\n");
    }
}

void handle_user_command(char *command, int client_socket, ClientSession *session)
{
    char username[50];
    sscanf(command, "USER %s", username);

    if (strcmp(username, "anonymous") != 0)
    {
        send_message(client_socket, "530 This FTP server is anonymous only.\r\n");
        return;
    }

    session->logged_in = 1;
    session->waiting_for_pass = 1;
    strcpy(session->username, username);
    send_message(client_socket, "331 Guest login ok, send your complete e-mail address as password.\r\n");
}

void handle_pass_command(char *command, int client_socket, ClientSession *session)
{
    char password[50];
    sscanf(command, "PASS %s", password);

    session->waiting_for_pass = 0;
    strcpy(session->password, password);
    send_message(client_socket, "230 Login successful.\r\n");
}
