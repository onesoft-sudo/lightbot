#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <json-c/json.h>

#define CONFIG_FILE_PATH getenv("CONFIG_FILE")

void config_init(char *config_file);
json_object *config_get();
const char *config_get_string(char *key);

#endif /* __CONFIG_H__ */