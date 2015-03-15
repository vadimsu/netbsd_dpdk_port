
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <rte_common.h>
#include <rte_lcore.h>

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

int enxio()
{
    return 0;
}

void schednetisr(int num)
{
}

int get_current_cpu()
{
    return rte_lcore_id();
}
