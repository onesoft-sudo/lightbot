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

void command_create_shortcut(struct discord *client, const struct discord_message *message)
{
    strtok(message->content, " ");
    char *name = strtok(NULL, " ");

    if (name == NULL) {
        struct discord_create_message error_reply = { .content = "You must provide a name of the shortcut!" };
        discord_create_message(client, message->channel_id, &error_reply, NULL);
        return;
    }

    if (strlen(name) > 512) {
        struct discord_create_message error_reply = { .content = "The shortcut name must not have more than 512 characters!" };
        discord_create_message(client, message->channel_id, &error_reply, NULL);
        return;
    }

    char *content = strtok(NULL, " ");

    if (content == NULL) {
        struct discord_create_message error_reply = { .content = "You must provide the content of the shortcut!" };
        discord_create_message(client, message->channel_id, &error_reply, NULL);
        return;
    }

    if (strlen(name) > DISCORD_MAX_MESSAGE_LEN) {
        char error_reply_content[1024];
        snprintf(error_reply_content, sizeof error_reply_content, "The shortcut name must not have more than %d characters!", DISCORD_MAX_MESSAGE_LEN);

        struct discord_create_message error_reply = { .content = error_reply_content };
        discord_create_message(client, message->channel_id, &error_reply, NULL);
        return;
    }

    shortcut_create(name, content);

    char reply_content[1024];
    snprintf(reply_content, sizeof reply_content, "The shortcut has been created with name `%s`!", name);

    struct discord_create_message reply = { .content = reply_content };
    discord_create_message(client, message->channel_id, &reply, NULL);
}