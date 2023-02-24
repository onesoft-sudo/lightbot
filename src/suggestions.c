#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <stdbool.h>
#include <string.h>
#include <json-c/json.h>
#include "suggestions.h"
#include <uuid/uuid.h>

json_object *root;

void suggestions_init() {
    root = json_object_from_file(SUGGESTIONS_FILE_PATH == NULL ? "config/suggestions.json" : SUGGESTIONS_FILE_PATH);
}

bool create_suggestion(u64snowflake user_id, char *content) {
    json_object *suggestions = json_object_new_object();
    char user_id_buff[256];

    sprintf(user_id_buff, "%" PRIu64, user_id);

    json_object_object_add(suggestions, "user_id", json_object_new_string(user_id_buff));
    json_object_object_add(suggestions, "content", json_object_new_string(content));

    uuid_t binuuid;
    uuid_generate_random(binuuid);

    char *uuid = malloc(37);

    #ifdef capitaluuid
        uuid_unparse_upper(binuuid, uuid);
    #elif lowercaseuuid
        uuid_unparse_lower(binuuid, uuid);
    #else
        uuid_unparse(binuuid, uuid);
    #endif

    json_object_object_add(root, uuid, suggestions);

    if (json_object_to_file(SUGGESTIONS_FILE_PATH == NULL ? "config/suggestions.json" : SUGGESTIONS_FILE_PATH, root) != 0) {
        log_error("Failed to save file: %s", SUGGESTIONS_FILE_PATH == NULL ? "config/suggestions.json" : SUGGESTIONS_FILE_PATH);
        return false;
    }

    return true;
}