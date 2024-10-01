#ifndef GLOBALS_H
#define GLOBALS_H

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

#define PATH_MAX_LEN 1024
#define CMD_MAX_LEN 1024

#define USERNAME_MAX_LEN 50
#define PASSWORD_MAX_LEN 50

extern int port;
extern char root_path[PATH_MAX_LEN];

extern int debug_mode;

#endif // GLOBALS_H
