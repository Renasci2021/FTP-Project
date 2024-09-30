#ifndef COMMANDS_H
#define COMMANDS_H

#include "session.h"

typedef void (*CommandHandler)(ClientSession *session, int client_socket, const char *command);

typedef struct
{
    const char *command;
    CommandHandler handler;
} CommandMapping;

void process_command(char *command, int client_socket);

void handle_user(ClientSession *session, int client_socket, const char *command);
void handle_pass(ClientSession *session, int client_socket, const char *command);
void handle_quit(ClientSession *session, int client_socket, const char *command);
void handle_unknown(ClientSession *session, int client_socket, const char *command);

#endif // COMMANDS_H
