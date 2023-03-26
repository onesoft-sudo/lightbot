/* suggestions.h -- suggestion management utility functions and types

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

#ifndef __SUGGESTIONS_H__
#define __SUGGESTIONS_H__

#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <json-c/json.h>

#define SUGGESTIONS_FILE_PATH getenv("SUGGESTIONS_FILE")

enum suggestion_status {
    SUGGESTION_PENDING,
    SUGGESTION_IMPLEMENTED,
    SUGGESTION_ACCEPTED,
    SUGGESTION_DENIED,
    SUGGESTION_ALREADY_SUGGESTED,
    SUGGESTION_ALREADY_IMPLEMENTED,
    SUGGESTION_INCOMPLETE,
    SUGGESTION_INVALID,
    SUGGESTION_UNKNOWN
};

void suggestions_init(char *suggestions_file);
bool suggestions_create(u64snowflake msg_id, u64snowflake user_id, char *content, enum suggestion_status status);
const char *suggestions_status_stringify(enum suggestion_status status);
const char *suggestions_status_stringify_description(enum suggestion_status status);
json_object *suggestions_get(char *id);
enum suggestion_status suggestions_status_from_string(char *str);
bool suggestions_save();
int suggestions_status_color(enum suggestion_status status);
bool suggestions_delete(char *id);

#endif /* __SUGGESTIONS_H__  */
