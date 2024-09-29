#include <stdio.h>
#include <stdarg.h>
#include "include/utils.h"
#include "include/globals.h"

void log_error(const char *format, ...)
{
    if (debug_mode)
    {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
    }
}

void log_info(const char *format, ...)
{
    if (debug_mode)
    {
        va_list args;
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
    }
}