/*
 commands.c -- define the command handler

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

#include <string.h>
#include <commands.h>

command_t *find_command(command_t **commands, char *name)
{
    int index = 0;
    int length = (sizeof commands) / sizeof (command_t);

    while (index < length) {
        command_t *command = commands[index];

        printf("%d Time\n", index);

        if (strcmp(command->name, name) == 0) {
            return commands[index];
        }

        index++;
    }
}