#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>

void command_setstatus(struct discord *client, const struct discord_message *event);

#endif