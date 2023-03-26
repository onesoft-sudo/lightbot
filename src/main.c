/* main.c -- the main executable program and argument parsing

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <stdbool.h>
#include <json-c/json.h>
#include <concord/discord.h>
#include <concord/log.h>

#include "commands.h"
#include "events.h"
#include "config.h"
#include "suggestions.h"
#include "common.h"
#include "server.h"

#include "../config.h"

#define BOT_TOKEN getenv("BOT_TOKEN")

struct discord *client;
json_object *config;

static void set_command_handlers() {
    discord_set_on_command(client, "setstatus", &command_setstatus);
    discord_set_on_command(client, "status", &command_status);
    discord_set_on_command(client, "showstatus", &command_showstatus);
    discord_set_on_command(client, "viewstatus", &command_showstatus);
    discord_set_on_command(client, "about", &command_about);
    discord_set_on_command(client, "delete", &command_delete);
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
    printf("LightBot version " VERSION "\n\
Copyright (C) 2023 OSN Inc.\n\
License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n");
}

static void print_help(char *argv_0) {
    printf("Usage: %s [OPTION]...\n\
A Discord Suggestion Management bot written in C.\n\
\n\
Options:\n\
  -h, --help                 Show this help and exit.\n\
  -p, --port=[PORT]          Specify the HTTP server port.\n\
  -t, --token=[TOKEN]        Specify a bot token manually.\n\
  -v, --version              Show version information.\n\
\n\
LightBot support email: <support@onesoftnet.eu.org>\n\
Discord Server (this server is not owned by OSN, it's for\n\
support cases or informal discussion about everything): <https://discord.gg/dKArUwBMKw>\
\n", argv_0);
}

void *server_routine() {
    server_init();
    return NULL;
}

void *concord_routine() {
    discord_run(client);
    return NULL;
}

int main(int argc, char **argv) {
    char *bot_token = NULL, *config_file_path = NULL, *suggestions_file_path = NULL;

    while (true) {
        const struct option long_options[] = {
            {"config", required_argument, 0, 'f'},
            {"help", no_argument, 0, 'h'},
            {"version", no_argument, 0, 'v'},
            {"token", required_argument, 0, 't'},
            {"port", required_argument, 0, 'p'},
            {"suggestions", required_argument, 0, 's'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int port_parsed;
        int c = getopt_long(argc, argv, "hvt:p:f:s:", long_options, &option_index);

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

            case 'p':
                port_parsed = atoi(optarg);

                if (!port_parsed) {
                    log_error("Invalid port specified.");
                    exit(EXIT_FAILURE);
                }

                server_set_port(port_parsed);
            break;

            case 'f':
                config_file_path = strdup(optarg);
            break;

            case 's':
                suggestions_file_path = strdup(optarg);
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

    config_init(config_file_path);
    suggestions_init(suggestions_file_path);
    
    if (bot_token == NULL)
        client = discord_init(BOT_TOKEN);
    else
        client = discord_init(bot_token);

    config = config_get();

    configure_client();
    set_event_handlers();
    set_command_handlers();

    pthread_t server_thread, concord_thread;

    pthread_create(&server_thread, NULL, &server_routine, NULL);
    pthread_create(&concord_thread, NULL, &concord_routine, NULL);
    pthread_join(server_thread, NULL);
    pthread_join(concord_thread, NULL);

    discord_cleanup(client);

    return 0;
}
