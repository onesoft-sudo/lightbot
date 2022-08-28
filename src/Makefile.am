bin_PROGRAMS = spark
spark_SOURCES = spark.c on_ready_event.c commands.c json_config.c
spark_LDFLAGS = -pthread
spark_LDADD = -lpthread -ljson-c -ldiscord -lcurl
spark_CPPFLAGS = -I$(top_srcdir)/include