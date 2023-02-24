CFLAGS = -L/usr/local/lib -I/usr/local/include

all:
	gcc -c -o commands.o src/commands.c
	gcc -c -o events.o src/events.c
	gcc -c -o suggestions.o src/suggestions.c
	gcc -c -o main.o src/main.c
	gcc -L./json-c-build -o main main.o commands.o events.o suggestions.o -ljson-c -luuid -pthread -ldiscord -lcurl 