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
        if (sessions[i].socket == client_socket)
        {
            return &sessions[i];
        }
    }
    return NULL; // 未找到会话
}

int add_session(int client_socket)
{
    if (client_count >= MAX_CLIENTS)
    {
        return -1; // 会话已满
    }

    sessions[client_count].socket = client_socket;
    memset(sessions[client_count].username, 0, 50);
    memset(sessions[client_count].password, 0, 50);
    sessions[client_count].logged_in = 0;
    sessions[client_count].waiting_for_pass = 0;

    client_count++;
    return 0;
}

void remove_session(int client_socket)
{
    for (int i = 0; i < client_count; i++)
    {
        if (sessions[i].socket == client_socket)
        {
            // 移动最后一个会话到当前位置
            sessions[i] = sessions[--client_count];
            break;
        }
    }
}
