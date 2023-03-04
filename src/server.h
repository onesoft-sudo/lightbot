/* server.h -- types and declarations for the functions related to the server

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

#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdlib.h>

/* The name of the server. Present in the "Server" header. */
#define SERVER_NAME "Lightbot"

/* Max threads for handing requests. */
#define MAX_THREADS 8

void server_set_port(int _port);
void server_init();
size_t server_thread_count();

#endif /* __SERVER_H__ */