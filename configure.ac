#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([lightbot], [0.0.1], [support@onesoftnet.eu.org])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/commands.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcurl':
AC_CHECK_LIB([curl], [curl_init])
# FIXME: Replace `main' with a function in `-ldiscord':
AC_CHECK_LIB([discord], [discord_run])
# FIXME: Replace `main' with a function in `-luuid':
AC_CHECK_LIB([uuid], [uuid_generate_random])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h getopt.h json-c/json_types.h json-c/json_object.h json-c/json.h concord/discord.h concord/types.h concord/log.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
