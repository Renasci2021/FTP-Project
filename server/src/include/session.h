#ifndef SESSION_H
#define SESSION_H

#include "globals.h"

typedef struct
{
    int socket;
    char username[50];
    char password[50];

    int is_connected; // 0: 未连接, 1: 已连接
    int logged_in; // 0: 未登录, 1: 已登录
    int expecting_pass; // 0: 未等待 PASS 命令, 1: 等待 PASS 命令
} ClientSession;

extern ClientSession sessions[MAX_CLIENTS];
extern int client_count;

ClientSession *find_session(int client_socket);
ClientSession *add_session(int client_socket);
void remove_session(int client_socket);

#endif
