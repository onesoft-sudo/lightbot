/* main.c -- the main executable program and argument parsing

   Copyright (C) 2022-2023 OSN Software Foundation, Inc.

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

#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <string.h>
#include <json-c/json.h>
#include <getopt.h>
#include "commands.h"
#include "events.h"
#include "config.h"
#include "suggestions.h"
#include "../include/ac_config.h"

#define BOT_TOKEN getenv("BOT_TOKEN")

struct discord *client;
json_object *config;

static void set_command_handlers() {
    discord_set_on_command(client, "setstatus", &command_setstatus);
    discord_set_on_command(client, "status", &command_setstatus);
}

static void set_event_handlers() {
    discord_set_on_ready(client, &on_ready);
    discord_set_on_message_create(client, &on_message);
}

static void configure_client() {
    discord_add_intents(client, DISCORD_GATEWAY_MESSAGE_CONTENT);
    discord_set_prefix(client, json_object_get_string(json_object_object_get(config, "prefix")));
}

static void print_version() {
    printf("LightBot version " VERSION "\
Copyright (C) 2023 OSN Inc.\
License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\
This is free software: you are free to change and redistribute it.\
There is NO WARRANTY, to the extent permitted by law.\n");
}

static void print_help(char *argv_0) {
    printf("Usage: %s [OPTION]...\n\
A Discord Suggestion Management bot written in C.\n\
\n\
Options:\n\
  -h, --help                 Show this help and exit.\n\
  -t, --token=[TOKEN]        Specify a bot token manually.\n\
  -v, --version              Show version information.\n\
\n\
LightBot support email: <support@onesoftnet.eu.org>\n\
Discord Server (this server is not owned by OSN, it's for\n\
support cases or informal discussion about everything): <https://discord.gg/tesworld>\
\n", argv_0);
}

int main(int argc, char **argv) {
    char *bot_token = NULL;

    while (true) {
        const struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"version", no_argument, 0, 'v'},
            {"token", required_argument, 0, 't'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hvt:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help(argv[0]);
                return 0;

            case 'v':
                print_version();
                return 0;

            case 't':
                bot_token = strdup(optarg);
            break;

            case '?':
            default:
                return -1;
        }
    }

    if (bot_token == NULL && BOT_TOKEN == NULL) {
        log_error("Bot token not found! Please set the `BOT_TOKEN' environment variable or specify a token with `--token' option.");
        return -1;
    }

    config_init();
    suggestions_init();
    
    if (bot_token == NULL)
        client = discord_init(BOT_TOKEN);
    else
        client = discord_init(bot_token);

    config = config_get();

    configure_client();
    set_event_handlers();
    set_command_handlers();

    discord_run(client);
    discord_cleanup(client);

    return 0;
}