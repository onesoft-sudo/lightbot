#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>

void command_suggest(struct discord *client, struct discord_message *event);

#endif