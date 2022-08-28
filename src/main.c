/*
* main.c -- entry point of the bot
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

bool test(struct discord *client, const struct discord_message *message, char **args)
{
    struct discord_create_message reply = {
            .content = "Test success!"
    };

    discord_create_message(client, message->channel_id, &reply, NULL);
}


void on_message_create(struct discord *client, const struct discord_message *message)
{
    if (message->author->bot) {
        return;
    }

    if (strcmp(message->content, "-ping") == 0) {
        struct discord_create_message reply = {
                .content = "Pong!"
        };

        discord_create_message(client, message->channel_id, &reply, NULL);
        return;
    }

    if (message->content[0] != '-') {
        return;
    }

    commands_t commands = {
            .commands = {
                    &test
            }
    };

    command_t *command = find_command(&commands, "test");

    if (command == NULL) {
        puts("Command not found");
    }
    else {
        char *argv = {"abc"};
        command->callback(client, message, &argv);
    }
}

int main()
{
    printf("Spark Bot!\n");

    struct discord *client = discord_init(getenv("TOKEN"));
    discord_set_on_ready(client, &on_ready);
    discord_set_on_message_create(client, &on_message_create);
    discord_run(client);

    return 0;
}
