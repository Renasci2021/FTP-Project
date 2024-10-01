#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#define DEFAULT_PORT 21
#define DEFAULT_ROOT "/tmp"

void print_help();
int parse_arguments(int argc, char *argv[], int *port, char *root);

#endif // ARG_PARSER_H
