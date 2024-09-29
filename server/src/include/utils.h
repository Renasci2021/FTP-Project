#ifndef UTILS_H
#define UTILS_H

void log_error(const char *format, ...);
void log_info(const char *format, ...);

void send_message(int client_socket, const char *message);

#endif // UTILS_H
