/*
 commands.h -- declare the commands

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
 along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef LIGHT_COMMANDS_H
#define LIGHT_COMMANDS_H

#include <stdbool.h>
#include <concord/discord.h>

void command_view_shortcut(struct discord *, const struct discord_message *);
void command_create_shortcut(struct discord *, const struct discord_message *);

#endif /* LIGHT_COMMANDS_H  */
