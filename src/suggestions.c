/* suggestions.c -- suggestion management utilities of the bot

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
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <json-c/json.h>

#include "suggestions.h"
#include "utils.h"
#include "common.h"

json_object *root;

void suggestions_init(char *suggestions_file) {
    char *suggestions_file_path = suggestions_file != NULL ? suggestions_file : SUGGESTIONS_FILE_PATH;
    
    root = json_object_from_file(suggestions_file_path == NULL ? "/usr/local/etc/" LIGHTBOT_CONFIG_DIR "/suggestions.json" : suggestions_file_path);

    if (!root) {
        root = json_object_from_file(suggestions_file_path == NULL ? "/usr/etc/" LIGHTBOT_CONFIG_DIR "/suggestions.json" : suggestions_file_path);
    }

    if (!root) {
        root = json_object_from_file(suggestions_file_path == NULL ? "/etc/" LIGHTBOT_CONFIG_DIR "/suggestions.json" : suggestions_file_path);
    }

    if (!root) {
        log_error("Could not open suggestions file: %s", strerror(errno));
        exit(-1);
    }
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
          
        case SUGGESTION_DENIED:
            return "Denied";
          
        default:
            return "Unknown";
    }
}

int suggestions_status_color(enum suggestion_status status) {
    switch (status) {
        case SUGGESTION_PENDING:
            return 0x007bff;
          
        case SUGGESTION_ACCEPTED:
            return 0x00ff4c;
            
        case SUGGESTION_ALREADY_IMPLEMENTED:
            return 0xf14a60;
          
        case SUGGESTION_ALREADY_SUGGESTED:
            return 0xf14a60;
          
        case SUGGESTION_IMPLEMENTED:
            return 0x03cffc;
          
        case SUGGESTION_INCOMPLETE:
            return 0xf14a60;
          
        case SUGGESTION_INVALID:
            return 0xfcca03;
          
        case SUGGESTION_DENIED:
            return 0xf14a60;
          
        default:
            return 0x000000;
    }
}

const char *suggestions_status_stringify_description(enum suggestion_status status) {
    switch (status) {
        case SUGGESTION_PENDING:
            return "This suggestion is waiting for a review";
          
        case SUGGESTION_ACCEPTED:
            return "This suggestion was accepted";
            
        case SUGGESTION_IMPLEMENTED:
            return "This suggestion was implemented";
          
        case SUGGESTION_ALREADY_SUGGESTED:
            return "This was already suggested";
          
        case SUGGESTION_ALREADY_IMPLEMENTED:
            return "This was already implemented";
          
        case SUGGESTION_INCOMPLETE:
            return "This suggestion is incomplete";
          
        case SUGGESTION_INVALID:
            return "This suggestion is invalid";
          
        case SUGGESTION_DENIED:
            return "This suggestion was denied";
          
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

bool suggestions_delete(char *id) {
    json_object_object_del(root, id);
    return suggestions_save();
}

enum suggestion_status suggestions_status_from_string(char *str) {
    char *lower = utils_strtolower(str);

    if (strcmp(lower, "pending") == 0) {
        return SUGGESTION_PENDING;
    }
    else if (strcmp(lower, "accepted") == 0) {
        return SUGGESTION_ACCEPTED;
    }
    else if (strcmp(lower, "implemented") == 0) {
        return SUGGESTION_IMPLEMENTED;
    }
    else if (strcmp(lower, "denied") == 0 || strcmp(lower, "rejected") == 0) {
        return SUGGESTION_DENIED;
    }
    else if (strcmp(lower, "invalid") == 0) {
        return SUGGESTION_INVALID;
    }
    else if (strcmp(lower, "incomplete") == 0) {
        return SUGGESTION_INCOMPLETE;
    }
    else if (strcmp(lower, "alreadysuggested") == 0 || strcmp(lower, "already-suggested") == 0 || strcmp(lower, "already_suggested") == 0 || strcmp(lower, "suggested_already") == 0) {
        return SUGGESTION_ALREADY_SUGGESTED;
    }
    else if (strcmp(lower, "alreadyimplemented") == 0 || strcmp(lower, "already-implemented") == 0 || strcmp(lower, "already_implemented") == 0 || strcmp(lower, "implemented_already") == 0) {
        return SUGGESTION_ALREADY_IMPLEMENTED;
    }
    else {
        return SUGGESTION_UNKNOWN;
    }
}
