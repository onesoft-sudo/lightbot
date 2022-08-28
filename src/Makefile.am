bin_PROGRAMS = spark
spark_SOURCES = main.c on_ready_event.c commands.c
spark_LDFLAGS = -pthread
spark_LDADD = -lpthread -ldiscord -lcurl
spark_CPPFLAGS = -I$(top_srcdir)/include