include_HEADERS = commands.h \
                  common.h \
                  json_config.h \
                  on_ready_event.h \
                  shortcuts.h

bin_PROGRAMS = light
light_SOURCES = light.c \
                on_ready_event.c \
                commands.c \
                json_config.c \
                common.c \
                shortcuts.c \
                $(include_HEADERS)
light_LDFLAGS = -pthread
light_LDADD = -lpthread -ljson-c -ldiscord -lcurl
AM_CPPFLAGS = -I$(top_srcdir)/include
