/*
 json_config.h -- define the config management functions

 Copyright (C) 2022 OSN Inc.

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

#include <json-c/json.h>
#include <stdio.h>
#include <config.h>
#include <stdbool.h>
#include <string.h>
#include <concord/types.h>
#include <json_config.h>

#define CONFIG_FILE_PATH "config/config.json"

bool json_config_init(json_object **config) 
{
    *config = json_object_from_file(CONFIG_FILE_PATH);
    return *config == NULL;
}

json_object *get_config_by_guild_id(json_object *config, u64snowflake id) 
{
    json_object_object_foreach(config, key, val) {
       if (id == atoll(key)) {
            return val;
        }
    }

    return NULL;
}