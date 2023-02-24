#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>

void on_ready(struct discord *client, const struct discord_ready *event) {
    log_info("Logged in as %s!", event->user->username);
}

void on_message(struct discord *client, const struct discord_message *event) {
    if (event->author->bot) {
        return;
    }
}