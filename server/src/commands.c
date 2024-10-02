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

    if (session->data_socket > 0)
    {
        close(session->data_socket);
        session->data_socket = 0;
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

    if (session->data_socket > 0)
    {
        close(session->data_socket);
        session->data_socket = 0;
    }

    int pasv_socket = create_passive_socket();
    if (pasv_socket < 0)
    {
        send_message(client_socket, "425 Can't open passive connection.\r\n");
        return;
    }

    struct sockaddr_in data_address;
    socklen_t data_address_len = sizeof(data_address);
    getsockname(pasv_socket, (struct sockaddr *)&data_address, &data_address_len);

    unsigned char *ip = (unsigned char *)&data_address.sin_addr.s_addr;
    unsigned char *port = (unsigned char *)&data_address.sin_port;

    session->pasv_socket = pasv_socket;
    session->data_mode = PASV_MODE;

    char response[256];
    sprintf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d).\r\n",
            ip[0], ip[1], ip[2], ip[3], port[0], port[1]);
    send_message(client_socket, response);
}

void handle_retr(ClientSession *session, int client_socket, const char *command)
{
    if (!session->is_logged_in)
    {
        send_message(client_socket, "530 Please login with USER and PASS.\r\n");
        return;
    }

    if (session->data_mode == 0)
    {
        send_message(client_socket, "425 Use PORT or PASV first.\r\n");
        return;
    }

    if (strlen(command) <= 5)
    {
        send_message(client_socket, "501 Syntax error in parameters or arguments.\r\n");
        return;
    }

    if (session->data_socket > 0)
    {
        close(session->data_socket);
        session->data_socket = 0;
        log_error("Data socket already open.\n");
        send_message(client_socket, "500 Internal error.\r\n");
        return;
    }

    // 创建数据连接
    if (create_data_connection(session) < 0)
    {
        send_message(client_socket, "425 Can't open data connection.\r\n");
        return;
    }

    // 打开文件
    char filename[PATH_MAX_LEN];
    sscanf(command, "RETR %s", filename);
    char filepath[PATH_MAX_LEN * 2];
    snprintf(filepath, PATH_MAX_LEN * 2, "%s/%s", session->working_directory, filename);
    log_info("[%d] Retrieving file: %s\n", client_socket, filepath);

    FILE *file = fopen(filepath, "rb"); // 以二进制只读方式打开文件
    if (file == NULL)
    {
        close_data_connection(session);
        send_message(client_socket, "550 File not found.\r\n");
        return;
    }

    send_message(client_socket, "150 Opening data connection.\r\n");

    // 发送文件内容
    char buffer[BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
    {
        int total_bytes_sent = 0;
        while (total_bytes_sent < bytes_read)
        {
            int bytes_sent = send(session->data_socket, buffer + total_bytes_sent, bytes_read - total_bytes_sent, 0);
            if (bytes_sent < 0)
            {
                fclose(file);
                close_data_connection(session);
                send_message(client_socket, "426 Connection closed; transfer aborted.\r\n");
            }
            total_bytes_sent += bytes_sent;
        }
        session->bytes_transferred += total_bytes_sent;
    }

    fclose(file);
    close_data_connection(session);

    if (ferror(file))
    {
        send_message(client_socket, "451 File read error.\r\n");
    }
    else
    {
        send_message(client_socket, "226 Transfer complete.\r\n");
    }
}

void handle_stor(ClientSession *session, int client_socket, const char *command)
{
    if (!session->is_logged_in)
    {
        send_message(client_socket, "530 Please login with USER and PASS.\r\n");
        return;
    }

    if (session->data_mode == 0)
    {
        send_message(client_socket, "425 Use PORT or PASV first.\r\n");
        return;
    }

    if (strlen(command) <= 5)
    {
        send_message(client_socket, "501 Syntax error in parameters or arguments.\r\n");
        return;
    }

    if (session->data_socket > 0)
    {
        close(session->data_socket);
        session->data_socket = 0;
        log_error("Data socket already open.\n");
        send_message(client_socket, "500 Internal error.\r\n");
        return;
    }

    // 创建数据连接
    if (create_data_connection(session) < 0)
    {
        send_message(client_socket, "425 Can't open data connection.\r\n");
        return;
    }

    // 打开文件
    char filename[PATH_MAX_LEN];
    sscanf(command, "STOR %s", filename);
    char filepath[PATH_MAX_LEN * 2];
    snprintf(filepath, PATH_MAX_LEN * 2, "%s/%s", session->working_directory, filename);
    log_info("[%d] Storing file: %s\n", client_socket, filepath);

    FILE *file = fopen(filepath, "wb"); // 以二进制写入模式打开文件
    if (file == NULL)
    {
        close_data_connection(session);
        send_message(client_socket, "550 File not found or permission denied.\r\n");
        return;
    }

    send_message(client_socket, "150 Opening data connection.\r\n");

    // 接收文件内容
    char buffer[BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(session->data_socket, buffer, BUFFER_SIZE, 0)) > 0)
    {
        int total_bytes_written = 0;
        while (total_bytes_written < bytes_received)
        {
            int bytes_written = fwrite(buffer + total_bytes_written, 1, bytes_received - total_bytes_written, file);
            if (bytes_written < 0)
            {
                fclose(file);
                close_data_connection(session);
                send_message(client_socket, "451 File write error.\r\n");
            }
            total_bytes_written += bytes_written;
        }
        session->bytes_transferred += total_bytes_written;
    }

    fclose(file);
    close_data_connection(session);

    if (bytes_received < 0)
    {
        send_message(client_socket, "426 Connection closed; transfer aborted.\r\n");
    }
    else
    {
        send_message(client_socket, "226 Transfer complete.\r\n");
    }
}

void handle_syst(ClientSession *session, int client_socket, const char *command)
{
    send_message(client_socket, "215 UNIX Type: L8\r\n");
}

void handle_type(ClientSession *session, int client_socket, const char *command)
{
    if (strlen(command) <= 5)
    {
        send_message(client_socket, "501 Syntax error in parameters or arguments.\r\n");
        return;
    }

    if (strncmp(command, "TYPE I", 6) == 0)
    {
        send_message(client_socket, "200 Type set to I.\r\n");
    }
    else
    {
        send_message(client_socket, "504 Command not implemented for that parameter.\r\n");
    }
}

void handle_quit(ClientSession *session, int client_socket, const char *command)
{
    if (session->pasv_socket > 0)
    {
        close(session->pasv_socket);
    }
    if (session->data_socket > 0)
    {
        close(session->data_socket);
    }

    session->is_connected = 0;
    send_message(client_socket, "221 Goodbye.\r\n");
}

void handle_abor(ClientSession *session, int client_socket, const char *command)
{
    handle_quit(session, client_socket, command);
}

void handle_unknown(ClientSession *session, int client_socket, const char *command)
{
    send_message(client_socket, "500 Unknown command.\r\n");
}