#ifndef LIGHT_COMMON_H
#define LIGHT_COMMON_H
#include <stdbool.h>

extern char const **global_argv;

void set_global_argv(char const **);
bool str_startswith(char *, char *);

#endif