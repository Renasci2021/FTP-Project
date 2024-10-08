#ifndef SESSION_H
#define SESSION_H

#include "globals.h"

#define PORT_MODE 1
#define PASV_MODE 2

typedef struct
{
    char username[USERNAME_MAX_LEN]; // 用户名
    char password[PASSWORD_MAX_LEN]; // 密码

    char working_directory[PATH_MAX_LEN]; // 当前工作目录

    char ip_address[16]; // IP 地址
    int port;            // 端口号

    int control_socket; // 控制连接
    int pasv_socket;    // PASV 模式监听套接字
    int data_socket;    // 数据连接

    int is_connected;   // 0: 未连接, 1: 已连接
    int is_logged_in;   // 0: 未登录, 1: 已登录
    int expecting_pass; // 0: 未等待 PASS 命令, 1: 等待 PASS 命令
    int data_mode;      // 0: 未指定, 1: PORT 模式, 2: PASV 模式

    long long bytes_transferred; // 传输字节数
} ClientSession;

extern ClientSession sessions[MAX_CLIENTS];
extern int client_count;

ClientSession *find_session(int client_socket);
ClientSession *add_session(int client_socket);
void remove_session(int client_socket);

#endif
