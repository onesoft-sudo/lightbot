#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <string.h>
#include <json-c/json.h>
#include "config.h"

json_object *config_root;

void config_init() {
    config_root = json_object_from_file(CONFIG_FILE_PATH == NULL ? "config/config.json" : CONFIG_FILE_PATH);

    if (!config_root) {
        log_error("Could not open the config file. Please make sure the config file exists.");
        exit(-1);
    }
}

json_object *config_get() {
    return config_root;
}

const char *config_get_string(char *key) {
    return json_object_get_string(json_object_object_get(config_root, key));
}