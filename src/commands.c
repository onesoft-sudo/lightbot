/* commands.c -- the bot commands

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

#include <concord/discord_codecs.h>
#include <concord/error.h>
#include <concord/types.h>
#include <json-c/json_object.h>
#include <json-c/json_types.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <string.h>
#include <time.h>

#include "suggestions.h"
#include "commands.h"
#include "config.h"
#include "common.h"

static void validate_message_id(struct discord *client, const struct discord_message *event) {
    for (int i = 0; i < strlen(event->content); i++) {
        char c = event->content[i];

        if (c == ' ') {
            break;
        }

        if (c != '0' && c != '1' && c != '2' &&
                c != '3' && c != '3' && c != '4' &&
                c != '5' && c != '6' && c != '7' &&
                c != '8' && c != '9') {
            struct discord_create_message params = { .content = ":x: Invalid message ID. Please provide a valid message ID." };
            discord_create_message(client, event->channel_id, &params, NULL);
            return;
        }
    }
}

static void get_suggestion_message(struct discord *client, const struct discord_message *event, u64snowflake message_id, u64snowflake channel_id, struct discord_messages *msgs) {
    struct discord_get_channel_messages params = { 
        .limit = 1,
        .around = message_id
    };

    struct discord_ret_messages ret = { .sync = msgs };

    discord_get_channel_messages(client, channel_id, &params, &ret);

    if (!msgs->size) {
        discord_messages_cleanup(msgs);
        struct discord_create_message params = { .content = ":x: No such suggestion message found." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }
}

/*
* The `setstatus` command. 
*
* Usage: <prefix>setstatus <message_id> <new_status>
*/
void command_setstatus(struct discord *client, const struct discord_message *event) {
    if (strlen(event->content) == 0) {
        struct discord_create_message params = { .content = ":x: Please provide the suggestion message ID." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }

    validate_message_id(client, event);

    const char *channel_id_string = config_get_string("channel");
    long long int channel_id = atoll(channel_id_string);

    char *id = strtok(event->content, " ");
    char *status_string = strtok(NULL, " ");

    if (status_string == NULL) {
        struct discord_create_message params = { .content = ":x: Invalid arguments specified. Please specify a new status to set." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }

    enum suggestion_status status = utils_status_from_string(status_string);

    if (status == SUGGESTION_UNKNOWN) {
        struct discord_create_message params = { .content = ":x: Invalid status specified." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }

    long long int message_id = atoll(id);

    struct discord_messages msgs = { 0 };

    get_suggestion_message(client, event, message_id, channel_id, &msgs);

    struct discord_embed embed = msgs.array[0].embeds->array[0];

    embed.fields->array[0].value = strdup(suggestions_status_stringify(status));
    embed.footer->text = strdup(suggestions_status_stringify_description(status));
    embed.color = suggestions_status_color(status);

    struct discord_edit_message edit_params = {
        .embeds = &(struct discord_embeds){
            .size = 1,
            .array = &embed,
        }
    };

    discord_edit_message(client, msgs.array[0].channel_id, msgs.array[0].id, &edit_params, NULL);
    discord_messages_cleanup(&msgs);

    printf("ID: %s\n", id);

    json_object *suggestion = suggestions_get(id);

    if (suggestion == NULL) {
        struct discord_create_message params = { .content = ":x: No such suggestion found." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }

    json_object_object_add(suggestion, "status", json_object_new_int(status));
    
    if (suggestions_save()) {
        struct discord_create_message params = { .content = "The status of the given suggestion has been changed." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }
    else {
        struct discord_create_message params = { .content = ":x: Failed to save suggestion." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }
}

void command_showstatus(struct discord *client, const struct discord_message *event) {
    if (strlen(event->content) == 0) {
        struct discord_create_message params = { .content = ":x: Please provide the suggestion message ID." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }

    validate_message_id(client, event);

    const char *channel_id_string = config_get_string("channel");
    long long int channel_id = atoll(channel_id_string);

    char *id = strtok(event->content, " ");
    long long int message_id = atoll(id);

    struct discord_messages msgs = { 0 };

    get_suggestion_message(client, event, message_id, channel_id, &msgs);

    struct discord_embed embed = { 0 }, suggestion_embed = msgs.array[0].embeds->array[0];
    char embed_json[4096];
    
    time_t seconds = msgs.array[0].timestamp / 1000;

    sprintf(embed_json, "{\
        \"author\": {\
            \"name\": \"Suggestion by %s\"\
        },\
        \"fields\": [\
            {\
                \"name\": \"Status\",\
                \"value\": \"%s\"\
            },\
            {\
                \"name\": \"Created At\",\
                \"value\": \"<t:%ld>\"\
            }\
        ]\
    }", suggestion_embed.author->name, suggestion_embed.fields->array[0].value, seconds);

    discord_embed_from_json(embed_json, sizeof embed_json, &embed);

    embed.color = 0x007bff;
    embed.description = strdup(suggestion_embed.description);
    embed.author->icon_url = strdup(suggestion_embed.author->icon_url);
    embed.timestamp = discord_timestamp(client);

    struct discord_create_message params = {
        .embeds = & (struct discord_embeds) {
            .size = 1,
            .array = &embed
        } 
    };

    discord_create_message(client, event->channel_id, &params, NULL);
    discord_messages_cleanup(&msgs);
}

void command_status(struct discord *client, const struct discord_message *event) {
    if (strlen(event->content) == 0) {
        struct discord_create_message params = { .content = ":x: Please provide the suggestion message ID." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }

    validate_message_id(client, event);

    strtok(strdup(event->content), " ");

    if (strtok(NULL, " ") == NULL) {
        command_showstatus(client, event);
    }
    else {
        command_setstatus(client, event);
    }
}