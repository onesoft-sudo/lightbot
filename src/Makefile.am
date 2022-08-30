bin_PROGRAMS = light
light_SOURCES = light.c on_ready_event.c commands.c json_config.c
light_LDFLAGS = -pthread
light_LDADD = -lpthread -ljson-c -ldiscord -lcurl
AM_CPPFLAGS = -I$(top_srcdir)/include