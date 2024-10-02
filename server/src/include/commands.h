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

void handle_port(ClientSession *session, int client_socket, const char *command);
void handle_pasv(ClientSession *session, int client_socket, const char *command);

void handle_retr(ClientSession *session, int client_socket, const char *command);
void handle_stor(ClientSession *session, int client_socket, const char *command);

void handle_syst(ClientSession *session, int client_socket, const char *command);
void handle_type(ClientSession *session, int client_socket, const char *command);
void handle_quit(ClientSession *session, int client_socket, const char *command);
void handle_abor(ClientSession *session, int client_socket, const char *command);

void handle_unknown(ClientSession *session, int client_socket, const char *command);

#define COMMAND_LIST             \
    COMMAND("USER", handle_user) \
    COMMAND("PASS", handle_pass) \
    COMMAND("PORT", handle_port) \
    COMMAND("PASV", handle_pasv) \
    COMMAND("RETR", handle_retr) \
    COMMAND("STOR", handle_stor) \
    COMMAND("SYST", handle_syst) \
    COMMAND("TYPE", handle_type) \
    COMMAND("QUIT", handle_quit) \
    COMMAND("ABOR", handle_abor)

#endif // COMMANDS_H
