/* server.c -- the HTTP server of the bot

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
#include <string.h>
#include <concord/log.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>

#include "server.h"

static int port = 4000, sockfd;
static char method[256], uri[CHAR_MAX], host[CHAR_MAX];

void server_set_port(int _port) {
    port = _port;
}

static void server_safe_exit() {
    close(sockfd);
    exit(-1);
}

void server_init() {
    int tmp = 1;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof (int));

    if (sockfd == -1) {
        log_error("cannot open socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (signal(SIGINT, &server_safe_exit) != 0) {
        log_error("cannot set SIGINT signal handler: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address_in, client_address;

    address_in.sin_family = AF_INET;
    address_in.sin_port = htons(port);
    address_in.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *) &address_in, sizeof address_in) != 0) {
        log_error("cannot bind address on port %d: %s\n", port, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) != 0) {
        log_error("failed to listen on port %d: %s\n", port, strerror(errno));
        exit(EXIT_FAILURE);
    }

    log_info("server listening on port %d\n", port);

    socklen_t len = sizeof client_address;

    for (;;) {
        int connfd = accept(sockfd, (struct sockaddr *) &client_address, &len);

        if (connfd == -1) {
            log_error("failed to receive connection: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        server_response(connfd);
    }
    
    close(sockfd);
}

static void server_response_set_headers(FILE *conn) {   
    fprintf(conn, "Connection: Close\n");
    fprintf(conn, "Server: " SERVER_NAME "\n");
    fprintf(conn, "Host: %s\n", host);
}

static void server_response_set_default_content_type_length(FILE *conn, int len) {   
    fprintf(conn, "Content-Type: application/json; charset='utf-8'\n");
    fprintf(conn, "Content-Length: %d\n", len);
}

static void server_response_set_code(FILE *conn, int status, char *statusText) {
    fprintf(conn, "HTTP/1.1 %d %s\n", status, statusText == NULL ? "" : statusText);
}

static void server_response_send(FILE *conn, int status, char *statusText, char *response) {
    server_response_set_code(conn, status, statusText);
    server_response_set_headers(conn);
    server_response_set_default_content_type_length(conn, (int) strlen(response));
    fprintf(conn, "\n%s", response);
    log_info("%s %s - %d %s", method, uri, status, statusText == NULL ? "" : statusText);
}

static void server_response(int connfd) {
    FILE *conn = fdopen(connfd, "a+");
    
    if (!conn) {
        log_error("failed to open incoming connection socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    fscanf(conn, "%s %s HTTP/1.1\n", method, uri);
    fscanf(conn, "Host: %s\n", host);

    if (strcmp(uri, "/") != 0) {
        server_response_send(conn, 404, "Not Found", "{\"error\": \"Not found\"}");
    }
    else {
        server_response_send(conn, 200, "OK", "{\"message\": \"API server is up.\"}");
    }

    fclose(conn);
}