/*
* light.c -- entry point of the bot
*
* Copyright (C) 2022 OSN Inc.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <concord/discord.h>
#include <stdlib.h>
#include <string.h>
#include <on_ready_event.h>
#include <commands.h>
#include <json-c/json.h>
#include <json_config.h>

json_object *config;

void on_message_create(struct discord *client, const struct discord_message *message)
{
    if (message->author->bot) {
        return;
    }

    json_object *guild_config = get_config_by_guild_id(config, message->guild_id);

    puts(json_object_to_json_string(guild_config));

    json_object *prefix_object;

    if (!json_object_object_get_ex(guild_config, "prefix", &prefix_object)) {
        puts("error parse");
        exit(-1);
    }

    const char *prefix = json_object_get_string(prefix_object);

    if (message->content[0] != prefix[0]) {
        puts("message does not contain prefix");
        return;
    }

    if (strcmp(message->content, "-ping") == 0) {
        struct discord_create_message reply = {
                .content = "Pong!"
        };

        discord_create_message(client, message->channel_id, &reply, NULL);
        return;
    }
}
    
int main()
{
    printf("Light Bot!\n");

    config = json_object_from_file("config/config.json");

    if (config == NULL) {
        puts("fatal error: failed to read config file");
        exit(-1);
    }

    puts(json_object_to_json_string(config));

    struct discord *client = discord_init(getenv("TOKEN"));
    discord_set_on_ready(client, &on_ready);
    discord_set_on_message_create(client, &on_message_create);
    discord_run(client);

    json_object_put(config);

    return 0;
}
