/* commands.c -- the bot commands

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

#include <json-c/json.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "suggestions.h"
#include "commands.h"
#include "config.h"
#include "common.h"
#include "../config.h"

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
* Sets or updates the status of a suggestion.
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

    json_object *suggestion = suggestions_get(id);

    if (suggestion == NULL) {
        struct discord_create_message params = { .content = ":x: No such suggestion found." };
        discord_create_message(client, event->channel_id, &params, NULL);
        return;
    }

    if (status == SUGGESTION_ACCEPTED) {
        struct discord_channel dm_channel = {0};
        struct discord_ret_channel ret_channel = {.sync = &dm_channel};
        struct discord_create_dm params = { .recipient_id = json_object_get_int64(json_object_object_get(suggestion, "user_id")) };

        u64snowflake dm_channel_id;

        if (CCORD_OK == discord_create_dm(client, &params, &ret_channel)) {
            dm_channel_id = dm_channel.id;
            discord_channel_cleanup(&dm_channel);

            struct discord_guild guild = { 0 };
            struct discord_ret_guild guild_ret = { .sync = &guild };

            discord_get_guild(client, event->guild_id, &guild_ret);

            char dm_content[1024];

            sprintf(dm_content, "**Your suggestion was accepted in %s**\nSuggestion ID: `%s`", guild.name, id);

            struct discord_ret_message ret_message = {.sync = DISCORD_SYNC_FLAG};
            struct discord_create_message dm_params = { .content = dm_content };

            discord_create_message(client, dm_channel_id, &dm_params, &ret_message);
            discord_guild_cleanup(&guild);
        }
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

/*
* The `showstatus` command. 
* Shows status of a suggestion.
*
* Usage: <prefix>showstatus <message_id>
*/
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

/*
* The `status` command. 
*
* Usage: <prefix>status <message_id> [new_status]
*
* If `new_status` is passed, then it behaves identical to `setstatus`. 
* Otherwise it will show the status just like `showstatus`.
*/
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

/*
* The `about` command. 
* Shows information about the bot.
*
* Usage: <prefix>about
*/
void command_about(struct discord *client, const struct discord_message *event) {
    struct discord_embed_fields fields = { 0 };

    fields.array = (struct discord_embed_field[]) {
        { "Version",  VERSION, true },
        { "Source Code",  "[Click here](" LIGHTBOT_GIT_REPO ")", true },
        { "Licensed Under",  "[GNU Affero General Public License v3](https://www.gnu.org/licenses/agpl-3.0.en.html)", true },
        { "Author", LIGHTBOT_AUTHOR_NAME, true },
        { "Support", PACKAGE_BUGREPORT, true },
        { "Written in", "GNU C", true }
    };

    fields.size = 6;
    fields.realsize = fields.size;

    struct discord_embed embed = {
        .author = &(struct discord_embed_author) {
            .name = PACKAGE_NAME
        },
        .description = "**A free and open source Discord Suggestion Management bot, especially created"
                       " for [The Everything Server](https://discord.gg/tesworld).**\n\n"
                       "This bot comes with ABSOLUTELY NO WARRANTY.\n"
                       "This is free software, and you are welcome to redistribute it under certain conditions.\n"
                       "See the [GNU Affero General Public License v3](https://www.gnu.org/licenses/agpl-3.0.en.html)"
                       " for more detailed information.",
        .fields = &fields,
        .color = 0x007bff,
        .footer = &(struct discord_embed_footer) {
            .text = "Copyright (C) OSN Inc 2022-2023."
        }
    };

    struct discord_embeds embeds = {
        .array = &embed,
        .size = 1,
        .realsize = 1
    };

    struct discord_create_message params = {
        .embeds = &embeds
    };

    discord_create_message(client, event->channel_id, &params, NULL);
}
