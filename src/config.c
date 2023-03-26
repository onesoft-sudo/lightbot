/* config.c -- the bot configuration management

   Copyright (C) 2022-2023 OSN Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <string.h>
#include <errno.h>
#include <json-c/json.h>

#include "config.h"
#include "common.h"

json_object *config_root;

void config_init(char *config_file) {
    char *config_file_path = config_file != NULL ? config_file : CONFIG_FILE_PATH;

    config_root = json_object_from_file(config_file_path == NULL ? "/usr/local/etc/" LIGHTBOT_CONFIG_DIR "/config.json" : config_file_path);

    if (!config_root) {
        config_root = json_object_from_file(config_file_path == NULL ? "/usr/etc/" LIGHTBOT_CONFIG_DIR "/config.json" : config_file_path);
    }

    if (!config_root) {
        config_root = json_object_from_file(config_file_path == NULL ? "/etc/" LIGHTBOT_CONFIG_DIR "/config.json" : config_file_path);
    }

    if (!config_root) {
        log_error("could not open the config file: %s", strerror(errno));
        exit(-1);
    }
}

json_object *config_get() {
    return config_root;
}

const char *config_get_string(char *key) {
    return json_object_get_string(json_object_object_get(config_root, key));
}
