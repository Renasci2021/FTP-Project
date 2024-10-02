#include <stdio.h>
#include <string.h>
#include "include/session.h"
#include "include/globals.h"

ClientSession sessions[MAX_CLIENTS];
int client_count = 0;

ClientSession *find_session(int client_socket)
{
    for (int i = 0; i < client_count; i++)
    {
        if (sessions[i].control_socket == client_socket)
        {
            return &sessions[i];
        }
    }
    return NULL; // 未找到会话
}

ClientSession *add_session(int client_socket)
{
    if (client_count >= MAX_CLIENTS)
    {
        return NULL; // 会话已满
    }

    memset(&sessions[client_count], 0, sizeof(ClientSession));
    sessions[client_count].control_socket = client_socket;
    sessions[client_count].is_connected = 1;
    strcpy(sessions[client_count].working_directory, root_path);

    return &sessions[client_count++];
}

void remove_session(int client_socket)
{
    for (int i = 0; i < client_count; i++)
    {
        if (sessions[i].control_socket == client_socket)
        {
            // 移动最后一个会话到当前位置
            sessions[i] = sessions[--client_count];
            break;
        }
    }
}
