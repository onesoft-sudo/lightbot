/* events.c -- the bot gateway event handlers

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

#include "utils.h"
#include "common.h"
#include "config.h"
#include "suggestions.h"

void on_ready(struct discord *client, const struct discord_ready *event) {
    log_info("Logged in as %s!", event->user->username);
}

void on_message(struct discord *client, const struct discord_message *event) {
    if (event->author->bot) {
        return;
    }

    const char *channel = config_get_string("channel");
    char event_channel_id[strlen(channel)];

    sprintf(event_channel_id, "%" PRIu64, event->channel_id);

    printf("%s\n", (char *) &event_channel_id);

    if (strcmp(event_channel_id, channel) == 0) {
        puts("Suggestion Channel");
        
        struct discord_delete_message delete_params = {
            .reason = "Suggestion message was removed and re-sent by the bot"
        };

        discord_delete_message(client, event->channel_id, event->id, &delete_params, NULL);

        const char *fmt = 
        
            "{"
                "   \"author\": {"
                "       \"name\": \"%s#%s\","
                "       \"icon_url\": \"https://cdn.discordapp.com/avatars/%" PRIu64 "/%s.jpeg\""
                "   },"
                "   \"color\": %d,"
                "   \"fields\": ["
                "       {"
                "           \"name\": \"Status\","
                "           \"value\": \"%s\""
                "       }"
                "   ],"
                "   \"footer\": {"
                "       \"text\": \"%s\""
                "   }"
            "}";

        char embed_json[DISCORD_MAX_MESSAGE_LEN + strlen(fmt)];
        char message_content_escaped[sizeof (embed_json) * 2];

        utils_escape_quotes(event->content, &message_content_escaped);

        sprintf(embed_json, fmt, event->author->username,
                event->author->discriminator, event->author->id,
                event->author->avatar, 0x007bff,
                suggestions_status_stringify(SUGGESTION_PENDING),
                suggestions_status_stringify_description(SUGGESTION_PENDING));

        printf("%s\n", embed_json);
        
        struct discord_embed embed = {0};

        discord_embed_from_json(embed_json, sizeof(embed_json), &embed);
        embed.timestamp = discord_timestamp(client);
        embed.description = strdup(event->content);

        struct discord_create_message create_params = {
            .embeds = &(struct discord_embeds){
                .size = 1,
                .array = &embed,
            }
        };

        struct discord_message msg;
        struct discord_ret_message ret = { .sync = &msg };

        discord_create_message(client, event->channel_id, &create_params, &ret);
        suggestions_create(msg.id, event->author->id, event->content, SUGGESTION_PENDING);
    }
}