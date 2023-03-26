/* utils.c -- the bot utility functions

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

#include <concord/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "common.h"
#include "utils.h"
#include "suggestions.h"

void utils_escape_quotes(char *str, char *dest) {
    int j = 0;
  
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '"') {
            dest[j] = '\\';
            dest[j + 1] = '"';

            j = j + 2;

            continue;
        }

        if (str[i] == '\\') {
            dest[j] = '\\';
            dest[j + 1] = '\\';

            j = j + 2;

            continue;
        }
    
        dest[j] = str[i]; 
        j++;
    }
}

char *utils_strtolower(char *str) {
    char *out = malloc(sizeof (str) + 1);

    assert(out != NULL);

    for (int i = 0; i < strlen(str); i++) {
        out[i] = tolower(str[i]);
    }

    return out;
}
