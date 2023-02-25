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

void set_command_handlers() {
    discord_set_on_command(client, "setstatus", &command_setstatus);
    discord_set_on_command(client, "status", &command_setstatus);
}

void set_event_handlers() {
    discord_set_on_ready(client, &on_ready);
    discord_set_on_message_create(client, &on_message);
}

void configure_client() {
    discord_add_intents(client, DISCORD_GATEWAY_MESSAGE_CONTENT);
    discord_set_prefix(client, json_object_get_string(json_object_object_get(config, "prefix")));
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
                puts("Help");
                return 0;

            case 'v':
                puts("Version " VERSION);
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