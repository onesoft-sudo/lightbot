#include <stdio.h>
#include <stdlib.h>
#include <concord/discord.h>
#include <concord/log.h>
#include <string.h>
#include <json-c/json.h>
#include "commands.h"
#include "events.h"

#define BOT_TOKEN getenv("BOT_TOKEN")
#define CONFIG_FILE_PATH getenv("CONFIG_FILE")

int main(int argc, char const **argv) {
    if (BOT_TOKEN == NULL) {
        log_error("Bot token not found! Please set the `BOT_TOKEN' environment variable.");
        return -1;
    }

    json_object *config_root = json_object_from_file(CONFIG_FILE_PATH == NULL ? "config/config.json" : CONFIG_FILE_PATH);

    if (!config_root) {
        log_error("Could not open the config file. Please make sure the config file exists.");
        return -1;
    }

    suggestions_init();
    
    struct discord *client = discord_init(BOT_TOKEN);

    discord_add_intents(client, DISCORD_GATEWAY_MESSAGE_CONTENT);
    discord_set_on_ready(client, &on_ready);
    discord_set_on_message_create(client, &on_message);

    discord_set_prefix(client, json_object_get_string(json_object_object_get(config_root, "prefix")));
    discord_set_on_command(client, "suggest", &command_suggest);

    discord_run(client);
    discord_cleanup(client);

    return 0;
}