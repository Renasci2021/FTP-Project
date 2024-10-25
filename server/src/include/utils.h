#ifndef UTILS_H
#define UTILS_H

void trim_crlf(char *str);

void log_error(const char *format, ...);
void log_info(const char *format, ...);

void send_message(int client_socket, const char *message);

int calculate_path(char *input, char *path);

#endif // UTILS_H
