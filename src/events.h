#ifndef __EVENTS_H__
#define __EVENTS_H__

#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>

void on_ready(struct discord *client, const struct discord_ready *event);
void on_message(struct discord *client, const struct discord_message *event);

#endif