/* config.c -- the bot configuration management

   Copyright (C) 2022-2023 OSN Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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