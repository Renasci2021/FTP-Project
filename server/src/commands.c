#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "include/commands.h"
#include "include/utils.h"
#include "include/session.h"

// 处理 FTP 命令
void process_command(char *command, int client_socket)
{
    ClientSession *session = find_session(client_socket);
    if (session == NULL)
    {
        log_error("Session not found for client socket %d\n", client_socket);
        return;
    }

    if (session->expecting_pass &&
        strncmp(command, "PASS", 4) != 0)
    {
        send_message(client_socket, "503 Bad sequence of commands.\r\n");
        return;
    }

    CommandMapping command_mappings[] = {
        {"USER", handle_user},
        {"PASS", handle_pass},
        {"QUIT", handle_quit},
        {NULL, NULL}};

    for (int i = 0; command_mappings[i].command != NULL; i++)
    {
        if (strncmp(command, command_mappings[i].command, strlen(command_mappings[i].command)) == 0)
        {
            command_mappings[i].handler(session, client_socket, command);
            return;
        }
    }

    handle_unknown(session, client_socket, command);
}

void handle_user(ClientSession *session, int client_socket, const char *command)
{
    if (session->logged_in)
    {
        send_message(client_socket, "530 Can't change user, user is already logged in.\r\n");
        return;
    }

    char username[50];
    sscanf(command, "USER %s", username);

    if (strcmp(username, "anonymous") != 0)
    {
        send_message(client_socket, "530 This FTP server is anonymous only.\r\n");
        return;
    }

    session->logged_in = 1;
    session->expecting_pass = 1;
    strcpy(session->username, username);
    send_message(client_socket, "331 Guest login ok, send your complete e-mail address as password.\r\n");
}

void handle_pass(ClientSession *session, int client_socket, const char *command)
{
    if (!session->expecting_pass)
    {
        send_message(client_socket, "503 Bad sequence of commands.\r\n");
        return;
    }

    char password[50];
    sscanf(command, "PASS %s", password);

    session->expecting_pass = 0;
    strcpy(session->password, password);
    send_message(client_socket, "230 Login successful.\r\n");
}

void handle_quit(ClientSession *session, int control_socket, const char *command)
{
    session->is_connected = 0;
    send_message(control_socket, "221 Goodbye.\r\n");
}

void handle_unknown(ClientSession *session, int control_socket, const char *command)
{
    send_message(control_socket, "500 Unknown command.\r\n");
}