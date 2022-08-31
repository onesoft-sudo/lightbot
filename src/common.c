#include <common.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char const **global_argv;

void set_global_argv(char const **argv)
{
    global_argv = argv;
}

bool str_startswith(char *str, char *pre)
{
    size_t lenpre = strlen(pre), lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}