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
    SUGGESTION_REJECTED,
    SUGGESTION_ALREADY_SUGGESTED,
    SUGGESTION_ALREADY_IMPLEMENTED,
    SUGGESTION_INCOMPLETE,
    SUGGESTION_INVALID,
    SUGGESTION_UNKNOWN
};

void suggestions_init();
bool suggestions_create(u64snowflake msg_id, u64snowflake user_id, char *content, enum suggestion_status status);
const char *suggestions_status_stringify(enum suggestion_status status);
const char *suggestions_status_stringify_description(enum suggestion_status status);
json_object *suggestions_get(char *id);
enum suggestion_status utils_status_from_string(char *str);
bool suggestions_save();

#endif