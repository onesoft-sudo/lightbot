/*
 commands.c -- define the commands

 Copyright (C) 2022 OSN Inc.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <stdlib.h>
#include <string.h>
#include <commands.h>
#include <shortcuts.h>
#include <concord/discord.h>

void command_view_shortcut(struct discord *client, const struct discord_message *message)
{
    strtok(message->content, " ");
    char *token = strtok(NULL, " ");

    if (token == NULL) {
        struct discord_create_message error_reply = { .content = "You must provide a name of a shortcut!" };
        discord_create_message(client, message->channel_id, &error_reply, NULL);
        return;
    }

    shortcut_t *shortcut = shortcut_find(token);

    if (shortcut == NULL) {
        puts("FAIL");
        struct discord_create_message error_reply = { .content = "That shortcut does not exist!" };
        discord_create_message(client, message->channel_id, &error_reply, NULL);
    }
    else {
        char text[DISCORD_MAX_MESSAGE_LEN];
        snprintf(text, sizeof text, "%s", shortcut->content);
        
        struct discord_create_message reply = { .content = text };
        discord_create_message(client, message->channel_id, &reply, NULL); 
        return;
    }
}   