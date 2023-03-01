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

#include <json-c/json_object.h>
#include <json-c/json_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <string.h>

#include "suggestions.h"
#include "commands.h"
#include "config.h"
#include "common.h"

void command_setstatus(struct discord *client, const struct discord_message *event) {
    if (strlen(event->content) == 0) {
        struct discord_create_message params = { .content = ":x: Please provide the suggestion message ID." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }

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

    const char *channel_id_string = config_get_string("channel");
    long long int channel_id = atoll(channel_id_string);

    char *id = strtok(event->content, " ");
    char *status_string = strtok(NULL, " ");

    if (status_string == NULL) {
        struct discord_create_message params = { .content = ":x: Invalid arguments specified. Please specify a new status to set." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }

    printf("Suggestion: %s\n", status_string);

    enum suggestion_status status = utils_status_from_string(status_string);

    if (status == SUGGESTION_UNKNOWN) {
        struct discord_create_message params = { .content = ":x: Invalid status specified." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }

    long long int message_id = atoll(id);

    printf("Channel: %lli\n", channel_id);

    struct discord_get_channel_messages params = { 
        .limit = 1,
        .around = message_id
    };

    struct discord_messages msgs = { 0 };
    struct discord_ret_messages ret = { .sync = &msgs };

    discord_get_channel_messages(client, channel_id, &params, &ret);

    if (!msgs.size) {
        discord_messages_cleanup(&msgs);
        struct discord_create_message params = { .content = ":x: No such suggestion message found." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }

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