/*
 json_config.h -- declare the config management functions

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
 along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef SPARK_CONFIG_H
#define SPARK_CONFIG_H

#include <stdbool.h>

#define SHORTCUTS_PATH "/.light/shortcuts.json"

typedef struct shortcut
{
    char *name;
    char *content;
} shortcut_t;


void shortcut_init();
void shortcut_loop();
void shortcut_free();
shortcut_t *shortcut_find(char *);
void shortcut_find_r(bool (*callback)(shortcut_t *, int), shortcut_t **ptr);
bool shortcut_create(char *, char *, char **);

#endif /* SPARK_CONFIG_H  */

