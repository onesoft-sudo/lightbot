#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include "suggestions.h"

void command_suggest(struct discord *client, struct discord_message *event) {
    create_suggestion(event->author->id, event->content);

    struct discord_create_message params = {
        .content = "Added suggestion"
    };

    discord_create_message(client, event->channel_id, &params, NULL);
}