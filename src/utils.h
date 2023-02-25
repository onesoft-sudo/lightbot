#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <json-c/json.h>

void utils_escape_quotes(char *str, char *dest);
char *utils_strtolower(char *str);

#endif