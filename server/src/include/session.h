#ifndef SESSION_H
#define SESSION_H

#include "globals.h"

typedef struct
{
    int socket;
    char username[50];
    char password[50];

    int logged_in; // 0: 未登录, 1: 已登录
    int waiting_for_pass; // 0: 未等待 PASS 命令, 1: 等待 PASS 命令
} ClientSession;

extern ClientSession sessions[MAX_CLIENTS];
extern int client_count;

ClientSession *find_session(int client_socket);
int add_session(int client_socket);
void remove_session(int client_socket);

#endif
