/* suggestions.c -- suggestion management utilities of the bot

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

#include <json-c/json_object.h>
#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <stdbool.h>
#include <string.h>
#include <json-c/json.h>
#include <uuid/uuid.h>

#include "suggestions.h"
#include "utils.h"
#include "common.h"

json_object *root;

void suggestions_init() {
    root = json_object_from_file(SUGGESTIONS_FILE_PATH == NULL ? "config/suggestions.json" : SUGGESTIONS_FILE_PATH);
}

const char *suggestions_status_stringify(enum suggestion_status status) {
    switch (status) {
        case SUGGESTION_PENDING:
            return "Pending";
          
        case SUGGESTION_ACCEPTED:
            return "Accepted";
            
        case SUGGESTION_ALREADY_IMPLEMENTED:
            return "Already Implemented";
          
        case SUGGESTION_ALREADY_SUGGESTED:
            return "Already Suggested";
          
        case SUGGESTION_IMPLEMENTED:
            return "Implemented";
          
        case SUGGESTION_INCOMPLETE:
            return "Suggestion Incomplete";
          
        case SUGGESTION_INVALID:
            return "Invalid";
          
        case SUGGESTION_REJECTED:
            return "Rejected";
          
        default:
            return "Unknown";
    }
}

const char *suggestions_status_stringify_description(enum suggestion_status status) {
    switch (status) {
        case SUGGESTION_PENDING:
            return "This suggestion is waiting for a review";
          
        case SUGGESTION_ACCEPTED:
            return "This suggestion was accepted";
            
        case SUGGESTION_ALREADY_IMPLEMENTED:
            return "This suggestion was implemented";
          
        case SUGGESTION_ALREADY_SUGGESTED:
            return "This was already suggested";
          
        case SUGGESTION_IMPLEMENTED:
            return "This was already implemented";
          
        case SUGGESTION_INCOMPLETE:
            return "This suggestion is incomplete";
          
        case SUGGESTION_INVALID:
            return "This suggestion is invalid";
          
        case SUGGESTION_REJECTED:
            return "This suggestion was rejected";
          
        default:
            return "Unknown";
    }
}

bool suggestions_save() {
    if (json_object_to_file(SUGGESTIONS_FILE_PATH == NULL ? "config/suggestions.json" : SUGGESTIONS_FILE_PATH, root) != 0) {
        log_error("Failed to save file: %s", SUGGESTIONS_FILE_PATH == NULL ? "config/suggestions.json" : SUGGESTIONS_FILE_PATH);
        return false;
    }

    return true;
}

bool suggestions_create(u64snowflake msg_id, u64snowflake user_id, char *content, enum suggestion_status status) {
    json_object *suggestions = json_object_new_object();
    char user_id_buff[256];

    sprintf(user_id_buff, "%" PRIu64, user_id);

    json_object_object_add(suggestions, "user_id", json_object_new_string(user_id_buff));
    json_object_object_add(suggestions, "content", json_object_new_string(content));
    json_object_object_add(suggestions, "status", json_object_new_int((int) status));

    // uuid_t binuuid;
    // uuid_generate_random(binuuid);

    // char *uuid = malloc(37);

    // #ifdef capitaluuid
    //     uuid_unparse_upper(binuuid, uuid);
    // #elif lowercaseuuid
    //     uuid_unparse_lower(binuuid, uuid);
    // #else
    //     uuid_unparse(binuuid, uuid);
    // #endif

    char id[256];
    sprintf(id, "%" PRIu64, msg_id);

    json_object_object_add(root, id, suggestions);

    return suggestions_save();
}

json_object *suggestions_get(char *id) {
    return json_object_object_get(root, id);
}

enum suggestion_status utils_status_from_string(char *str) {
    char *lower = utils_strtolower(str);

    if (strcmp(lower, "pending") == 0) {
        return SUGGESTION_PENDING;
    }
    else if (strcmp(lower, "accepted") == 0) {
        return SUGGESTION_ACCEPTED;
    }
    else if (strcmp(lower, "implemnted") == 0) {
        return SUGGESTION_IMPLEMENTED;
    }
    else if (strcmp(lower, "rejected") == 0) {
        return SUGGESTION_REJECTED;
    }
    else if (strcmp(lower, "invalid") == 0) {
        return SUGGESTION_INVALID;
    }
    else if (strcmp(lower, "incomplete") == 0) {
        return SUGGESTION_INCOMPLETE;
    }
    else if (strcmp(lower, "alreadysuggested") == 0 || strcmp(lower, "already_suggested") == 0 || strcmp(lower, "suggested") == 0) {
        return SUGGESTION_ALREADY_SUGGESTED;
    }
    else if (strcmp(lower, "alreadyimplemented") == 0 || strcmp(lower, "already_implemented") == 0 || strcmp(lower, "implemented") == 0) {
        return SUGGESTION_ALREADY_IMPLEMENTED;
    }
    else {
        return SUGGESTION_UNKNOWN;
    }
}