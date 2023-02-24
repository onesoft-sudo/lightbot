#ifndef __SUGGESTIONS_H__
#define __SUGGESTIONS_H__

#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>

#define SUGGESTIONS_FILE_PATH getenv("SUGGESTIONS_FILE")

void suggestions_init();
bool suggestions_create(u64snowflake user_id, char *content);

#endif