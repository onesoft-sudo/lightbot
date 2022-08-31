include_HEADERS = ../include/commands.h \
                  ../include/common.h \
                  ../include/json_config.h \
                  ../include/on_ready_event.h \
                  ../include/shortcuts.h

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
