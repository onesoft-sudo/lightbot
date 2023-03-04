bin_PROGRAMS = light

light_HEADERS_CUSTOM = commands.h config.h events.h suggestions.h utils.h server.h
light_SOURCES = commands.c main.c config.c events.c suggestions.c utils.c server.c $(light_HEADERS_CUSTOM)
light_LDADD = -ljson-c -ldiscord -lcurl 
light_LDFLAGS = -pthread