#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <common.h>
#include <json-c/json.h>
#include <shortcuts.h>
#include <json_config.h>
#include <string.h>

json_object *shortcut_json;
size_t length = 0;

void shortcut_init() 
{
    char *filepath = malloc((strlen(getenv("HOME")) + strlen(SHORTCUTS_PATH)) * sizeof (char));
    sprintf(filepath, "%s%s", strdup(getenv("HOME")), SHORTCUTS_PATH);

    shortcut_json = json_object_from_file(filepath);

    FILE *config_fp = fopen(filepath, "r");

    if (!config_fp) {
        fprintf(stderr, "%s: failed to open file: %s: %s\n", global_argv[0], filepath, strerror(errno));
        free(filepath);
        exit(-1);
    }

    fclose(config_fp);
    free(filepath);

    if (shortcut_json == NULL) {
        fprintf(stderr, "%s: failed to parse config file; check the JSON syntax.\n", global_argv[0]);
        exit(-1);
    }

    length = json_object_array_length(shortcut_json);
    printf("len: %d\n", length);
}

void shortcut_find_r(bool (*callback)(shortcut_t *, int), shortcut_t **ptr)
{
    for (int i = 0; i < length; i++) {
        json_object *val = json_object_array_get_idx(shortcut_json, i);
        json_object *name_obj, *content_obj;

        if (!json_object_object_get_ex(val, "name", &name_obj)) {
            fprintf(stderr, "%s: malformed database\n", global_argv[0]);
            exit(-1);
        }

        if (!json_object_object_get_ex(val, "content", &content_obj)) {
            fprintf(stderr, "%s: malformed database\n", global_argv[0]);
            exit(-1);
        }

        const char *name = json_object_get_string(name_obj);
        const char *content = json_object_get_string(content_obj);

        shortcut_t tmp = { .name = strdup(name), .content = strdup(content) };

        if (callback(&tmp, i)) {
            *ptr = &tmp;
            return;
        }
    }

    *ptr = NULL;
}

void shortcut_free()
{
    json_object_put(shortcut_json);
}

void shortcut_loop()
{
    
}

char *shortcut_find_name = NULL;

static bool shortcut_find_callback(shortcut_t *shortcut, int index) 
{
    if (strcmp(shortcut->name, strdup(shortcut_find_name)) == 0) {
        return true;
    } 

    return false;
}

shortcut_t *shortcut_find(char *name)
{
    shortcut_find_name = strdup(name);
    shortcut_t *tmp = NULL;

    shortcut_find_r(&shortcut_find_callback, &tmp);
    return tmp;
}

bool shortcut_create(char *name, char *content)
{
    if (name == NULL || content == NULL) 
        return false;

    json_object *shortcut = json_object_new_object();
    json_object *name_obj = json_object_new_string(name);
    json_object *content_obj = json_object_new_string(content);

    json_object_object_add(shortcut, "name", name_obj);
    json_object_object_add(shortcut, "content", content_obj);

    json_object_array_add(shortcut_json, shortcut);

    char *filepath = malloc((strlen(getenv("HOME")) + strlen(SHORTCUTS_PATH)) * sizeof (char));
    sprintf(filepath, "%s%s", strdup(getenv("HOME")), SHORTCUTS_PATH);

    json_object_to_file(filepath, shortcut_json);

    free(filepath);
    length++;

    return true;
}