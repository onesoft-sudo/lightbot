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
#include <errno.h>
#include <common.h>
#include <shortcuts.h>

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
        fprintf(stderr, "%s: no prefix specified in the config\n", global_argv[0]);
        exit(-1);
    }

    const char *prefix = json_object_get_string(prefix_object);

    if (message->content[0] != prefix[0]) {
        return;
    }
    
    if (strcmp(message->content, "-ping") == 0) {
        struct discord_create_message reply = {
                .content = "Pong!"
        };

        discord_create_message(client, message->channel_id, &reply, NULL);
        return;
    }

    if (str_startswith(message->content, "-view")) {
        puts("here");
        command_view_shortcut(client, message);
    }
}

bool callback(shortcut_t *tmp, int index)
{
    if (strcmp(tmp->name, strdup("abc")) == 0) {
        printf("%s\n", tmp->content);
        return true;
    }

    return false;
}
    
int main(int argc, char const **argv)
{
    printf("Light Bot!\n");
    set_global_argv(argv);

    FILE *config_fp = fopen(CONFIG_FILE_PATH, "r");

    if (!config_fp) {
        fprintf(stderr, "%s: failed to open file %s: %s\n", argv[0], CONFIG_FILE_PATH, strerror(errno));
        exit(-1);
    }

    fclose(config_fp);

    bool success = json_config_init(&config);

    if (!success) {
        fprintf(stderr, "%s: failed to parse config file; check the JSON syntax.\n", argv[0]);
        exit(-1);
    }

    puts(json_object_to_json_string(config));

    shortcut_init();
    shortcut_loop();

    struct discord *client = discord_init(getenv("TOKEN"));
    discord_set_on_ready(client, &on_ready);
    discord_set_on_message_create(client, &on_message_create);
    discord_run(client);

    shortcut_free();
    json_object_put(config);

    return 0;
}
