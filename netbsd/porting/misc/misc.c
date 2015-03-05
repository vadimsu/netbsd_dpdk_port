
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

const char hexdigits[] = "0123456789abcdef";

void log_internal(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf(fmt,args);
}

void exit_internal(int code)
{
    exit(code);
}
