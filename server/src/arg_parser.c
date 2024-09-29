#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/arg_parser.h"

#define DEFAULT_PORT 21
#define DEFAULT_ROOT "/tmp"

void print_help()
{
    printf("Usage:\n");
    printf("  server [-port n] [-root /path/to/file/area]\n");
    printf("Options:\n");
    printf("  -port n       Set the TCP port number (default: 21)\n");
    printf("  -root path    Set the root directory for requests (default: /tmp)\n");
    printf("  -help         Display this help message\n");
}

int parse_arguments(int argc, char *argv[], int *port, char **root)
{
    *port = DEFAULT_PORT;
    *root = DEFAULT_ROOT;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-port") == 0 && i + 1 < argc)
        {
            *port = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-root") == 0 && i + 1 < argc)
        {
            *root = argv[++i];
        }
        else if (strcmp(argv[i], "-help") == 0)
        {
            print_help();
            return 0;
        }
        else
        {
            fprintf(stderr, "Invalid argument: %s\n", argv[i]);
            print_help();
            return -1;
        }
    }
    return 1;
}
