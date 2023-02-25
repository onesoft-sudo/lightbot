#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <string.h>
#include <json-c/json.h>
#include "commands.h"
#include "events.h"
#include "config.h"
#include "suggestions.h"

#define BOT_TOKEN getenv("BOT_TOKEN")

struct discord *client;
json_object *config_root;

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
    discord_set_prefix(client, json_object_get_string(json_object_object_get(config_root, "prefix")));
}

int main(int argc, char const **argv) {
    if (BOT_TOKEN == NULL) {
        log_error("Bot token not found! Please set the `BOT_TOKEN' environment variable.");
        return -1;
    }

    config_init();
    suggestions_init();
    
    client = discord_init(BOT_TOKEN);
    config_root = config_get();

    configure_client();
    set_event_handlers();
    set_command_handlers();

    discord_run(client);
    discord_cleanup(client);

    return 0;
}