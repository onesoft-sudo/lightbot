#include <concord/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <string.h>
#include "utils.h"
#include "suggestions.h"
#include <ctype.h>
#include <assert.h>

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