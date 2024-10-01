#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "include/commands.h"
#include "include/server.h"
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
        strncmp(command, "PASS", 4) != 0 &&
        strncmp(command, "QUIT", 4) != 0)
    {
        send_message(client_socket, "503 Bad sequence of commands.\r\n");
        return;
    }

#define COMMAND(cmd, handler) {cmd, handler},
    CommandMapping command_mappings[] = {
        COMMAND_LIST // 展开宏列表
        {NULL, NULL} // 结束标记
    };
#undef COMMAND

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
    if (session->is_logged_in)
    {
        send_message(client_socket, "530 Can't change user, user is already logged in.\r\n");
        return;
    }

    if (strlen(command) <= 5)
    {
        send_message(client_socket, "501 Username required.\r\n");
        return;
    }

    if (strlen(command) >= USERNAME_MAX_LEN)
    {
        send_message(client_socket, "501 Username too long.\r\n");
        return;
    }

    char username[USERNAME_MAX_LEN];
    sscanf(command, "USER %s", username);

    if (strcmp(username, "anonymous") != 0)
    {
        send_message(client_socket, "530 This FTP server is anonymous only.\r\n");
        return;
    }

    session->is_logged_in = 1;
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

    if (strlen(command) <= 5)
    {
        send_message(client_socket, "501 Password required.\r\n");
        return;
    }

    if (strlen(command) >= PASSWORD_MAX_LEN)
    {
        send_message(client_socket, "501 Password too long.\r\n");
        return;
    }

    char password[PASSWORD_MAX_LEN];
    sscanf(command, "PASS %s", password);

    session->expecting_pass = 0;
    strcpy(session->password, password);
    send_message(client_socket, "230 Login successful.\r\n");
}

void handle_port(ClientSession *session, int client_socket, const char *command)
{
    if (!session->is_logged_in)
    {
        send_message(client_socket, "530 Please login with USER and PASS.\r\n");
        return;
    }

    if (session->is_data_socket_open)
    {
        close(session->data_socket);
        session->is_data_socket_open = 0;
    }

    if (strlen(command) <= 5)
    {
        send_message(client_socket, "501 Syntax error in parameters or arguments.\r\n");
        return;
    }

    unsigned int ip1, ip2, ip3, ip4, port1, port2;
    if (sscanf(command, "PORT %u,%u,%u,%u,%u,%u", &ip1, &ip2, &ip3, &ip4, &port1, &port2) != 6)
    {
        send_message(client_socket, "501 Syntax error in parameters or arguments.\r\n");
        return;
    }

    if (ip1 > 255 || ip2 > 255 || ip3 > 255 || ip4 > 255 || port1 > 255 || port2 > 255)
    {
        send_message(client_socket, "501 Invalid IP address or port number.\r\n");
        return;
    }

    sprintf(session->ip_address, "%u.%u.%u.%u", ip1, ip2, ip3, ip4);
    session->port = port1 * 256 + port2;
    session->data_mode = PORT_MODE;
    send_message(client_socket, "200 PORT command successful.\r\n");
}

void handle_pasv(ClientSession *session, int client_socket, const char *command)
{
    if (!session->is_logged_in)
    {
        send_message(client_socket, "530 Please login with USER and PASS.\r\n");
        return;
    }

    if (session->is_data_socket_open)
    {
        close(session->data_socket);
        session->is_data_socket_open = 0;
    }

    int data_socket = create_passive_socket();
    if (data_socket < 0)
    {
        send_message(client_socket, "425 Can't open passive connection.\r\n");
        return;
    }

    struct sockaddr_in data_address;
    socklen_t data_address_len = sizeof(data_address);
    getsockname(data_socket, (struct sockaddr *)&data_address, &data_address_len);

    unsigned char *ip = (unsigned char *)&data_address.sin_addr.s_addr;
    unsigned char *port = (unsigned char *)&data_address.sin_port;

    session->data_socket = data_socket;
    session->data_mode = PASV_MODE;
    session->is_data_socket_open = 1;

    char response[256];
    sprintf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d).\r\n",
            ip[0], ip[1], ip[2], ip[3], port[0], port[1]);
    send_message(client_socket, response);
}

void handle_quit(ClientSession *session, int control_socket, const char *command)
{
    // TODO: 关闭数据连接
    session->is_connected = 0;
    send_message(control_socket, "221 Goodbye.\r\n");
}

void handle_unknown(ClientSession *session, int control_socket, const char *command)
{
    send_message(control_socket, "500 Unknown command.\r\n");
}