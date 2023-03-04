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
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <concord/log.h>
#include <netinet/in.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#include "server.h" /* MAX_THREADS */
#include "../config.h"

/* The server port, stored here for accessing from anywhere in this file. */
static int port = 4000;

/* The server socket, initialized by server_init(). */
static int sockfd;

/* These variables store method, request URI and hostname from the 
   incoming HTTP request. */
static char method[256], uri[CHAR_MAX], host[CHAR_MAX];

/* Threads for handing incoming requests in parallel. */
static pthread_t threads[MAX_THREADS];

/* Total threads count. */
static size_t thread_count = 0;

/* Return the appropriate error message depending on the value of `errno`. */
char *server_error() {
    return strerror(errno);
}

/* Since `thread_count` is static, it cannot be accessed outside of this file.
   This function allows to access the variable outside of this file. */
size_t server_thread_count() {
    return thread_count;
}

/* Set the server port. This function is used everywhere to set the port
   instead of directly modifying the port variable. */
void server_set_port(int _port) {
    port = _port;
}

/* Close the server socket and exit. */
static void server_safe_exit() {
    close(sockfd);
    exit(-1);
}

/* In case if we receive SIGINT signal, instead of directly exiting, close 
   the server socket first and then exit. */
static void server_safe_exit_sigint() {
    log_warn("force exit by user");
    server_safe_exit();
}

/* Write the common headers that needs to be sent with every response. */
static void server_response_set_headers(FILE *restrict conn) {   
    /* Determind the current time in GMT timezone. */
    time_t current_time = time(NULL);
    struct tm *timeinfo = gmtime(&current_time);

    /* Store the formatted string. */
    char datebuf[512];
    strftime(datebuf, sizeof datebuf, "%a, %d %b %Y %H:%M:%S %Z", timeinfo);

    /* Send headers back. */
    fprintf(conn, "Connection: Close\n");
    fprintf(conn, "Server: " SERVER_NAME "\n");
    fprintf(conn, "Host: %s\n", host);
    fprintf(conn, "Date: %s\n", datebuf);
}

/* Write the default content type header and content length header. Default content
   type is application/json. */
static void server_response_set_default_content_type_length(FILE *restrict conn, int len) {   
    fprintf(conn, "Content-Type: application/json; charset='utf-8'\n");
    fprintf(conn, "Content-Length: %d\n", len);
}

/* Write the response status code and status text. */
static void server_response_set_code(FILE *restrict conn, int status, char *statusText) {
    fprintf(conn, "HTTP/1.1 %d %s\n", status, statusText == NULL ? "" : statusText);
}

/* A shorthand for sending a simple string response with all the default settings. */
static void server_response_send(FILE *restrict conn, int status, char *statusText, char *response) {
    server_response_set_code(conn, status, statusText);
    server_response_set_headers(conn);
    server_response_set_default_content_type_length(conn, (int) strlen(response));
    fprintf(conn, "\n%s", response);
    log_info("%s %s - %d %s", method, uri, status, statusText == NULL ? "" : statusText);
}

/* Handle the incoming request and send a response back. In most cases, the server
   will send JSON responses. */
static void server_response(int connfd) {
    /* Open the incoming socket file descriptor (client socket). */
    FILE *conn = fdopen(connfd, "a+");
    
    if (!conn) {
        log_error("failed to open incoming connection socket: %s\n", server_error());
        exit(EXIT_FAILURE);
    }

    /* Scan the HTTP method, request URI and hostname from the socket connection. */
    (void) fscanf(conn, "%s %s HTTP/1.1\n", method, uri);
    (void) fscanf(conn, "Host: %s\n", host);

    /* For now, handle the root URI (/) only. In every other case, return 404. */
    if (strcmp(uri, "/") != 0) {
        server_response_send(conn, 404, "Not Found", "{\"error\": \"Not found\"}");
    }
    else {
        /* Store the system information. */
        struct sysinfo system_info;

        /* Attempt to get the system information. */
        if (sysinfo(&system_info) != 0) {
            log_warn("failed to get system info: %s", server_error());
            server_response_send(conn, 500, "Internal Server Error", "{\"error\": \"Internal error\"}");
        }
        else {
            /* Calculate the days, hours, minutes and seconds and then send a response back. */
            char *response_fmt = "{\"message\": \"API server is up.\",\"uptime\":\"%d days %d hours %d minutes %d seconds\",\"version\":\"" VERSION "\"}";
            char response[strlen(response_fmt) + 512];
            
            int days = round(system_info.uptime / (60 * 60 * 24));
            int hours = round((system_info.uptime - (days * 60 * 60 * 24)) / (60 * 60));
            int minutes = round((system_info.uptime - (days * 60 * 60 * 24) - (hours * 60 * 60)) / 60);
            int seconds = round((system_info.uptime - (days * 60 * 60 * 24) - (hours * 60 * 60) - (minutes * 60)));

            sprintf(response, response_fmt, days, hours, minutes, seconds);
            server_response_send(conn, 200, "OK", response);
        }
    }

    /* Close the connection. */
    fclose(conn);
}

/* The thread routine function for sending the response. */
static void *server_response_routine(void *arg) {
    int connfd = *((int *) arg);
    server_response(connfd);
    thread_count--;
    return NULL;
}

/* Creates a new thread for the incoming request. Also checks if the max thread
   limit is exceeded. */
static void server_response_thread_create(int connfd) {
    if (thread_count > MAX_THREADS) {
        log_error("maximum thread count exceeded: max count is %llu but attempted to create %llu threads", MAX_THREADS, thread_count);
        server_safe_exit();
    }

    if (pthread_create(&threads[thread_count++], NULL, &server_response_routine, (void *) &connfd) != 0) {
        log_error("failed to create thread for incoming request: %s\n", server_error());
        exit(EXIT_FAILURE);
    }
}

/* Initialize the server.
   Create the server socket, bind address and listen to the incoming requests. */
void server_init() {
    /* This is a temporary unused variable to store the value set by setsockopt(). */
    int tmp = 1;

    /* Store the server address information to bind the port and listen for requests. */
    struct sockaddr_in server_addr_in;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof (int));

    if (sockfd == -1) {
        log_error("cannot open socket: %s\n", server_error());
        exit(EXIT_FAILURE);
    }

    /* Signal action for handing SIGINT. */
    struct sigaction action = { 0 };

    action.sa_sigaction = &server_safe_exit_sigint;
    action.sa_flags = 0;

    if (sigaction(SIGINT, &action, NULL) != 0) {
        log_error("cannot set SIGINT signal handler: %s\n", server_error());
        exit(EXIT_FAILURE);
    }

    /* Set the information about the server socket address. */
    server_addr_in.sin_family = AF_INET;
    server_addr_in.sin_port = htons(port);
    server_addr_in.sin_addr.s_addr = INADDR_ANY;

    /* Bind the socket address. */
    if (bind(sockfd, (struct sockaddr *) &server_addr_in, sizeof server_addr_in) != 0) {
        log_error("cannot bind address on port %d: %s\n", port, server_error());
        exit(EXIT_FAILURE);
    }

    /* Listen to the given port. */
    if (listen(sockfd, 5) != 0) {
        log_error("failed to listen on port %d: %s\n", port, server_error());
        exit(EXIT_FAILURE);
    }

    log_info("server listening on port %d", port);

    /* This is used to store client information. */
    struct sockaddr_in client_addr;

    /* Size of the `client_addr` struct. */
    socklen_t len = sizeof client_addr;

    /* Loop infinitely and keep accepting incoming requests. */
    for (;;) {
        int connfd = accept(sockfd, (struct sockaddr *) &client_addr, &len);

        if (connfd == -1) {
            log_error("failed to receive connection: %s\n", server_error());
            exit(EXIT_FAILURE);
        }

        server_response_thread_create(connfd);
    }
    
    close(sockfd); 
}
