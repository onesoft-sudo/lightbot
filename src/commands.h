/* commands.h -- command function declarations

   Copyright (C) 2022-2023 OSN Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>

#define COMMAND_PARAMS struct discord *client, const struct discord_message *event

void command_status(COMMAND_PARAMS);
void command_showstatus(COMMAND_PARAMS);
void command_setstatus(COMMAND_PARAMS);
void command_about(COMMAND_PARAMS);
void command_delete(COMMAND_PARAMS);

#endif /* __COMMANDS_H__  */
