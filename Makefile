CC=gcc
CFLAGS=-Wall -Werror
LIBS=-lSDL2 -lSDL2_ttf
DEPS=main.c
OUTPUT=a.out

build:
	$(CC) $(CFLAGS) -o $(OUTPUT) $(DEPS) $(LIBS)

run: build
	./$(OUTPUT)

