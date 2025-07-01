# Makefile

CC=gcc
CFLAGS=-Wall -Ilibs/filesystem -Ilibs/user
BIN_DIR=bin

all: $(BIN_DIR)/main

$(BIN_DIR)/main: main.o filesystem.o user.o | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/main main.o filesystem.o user.o

main.o: main.c libs/filesystem/filesystem.h libs/user/user.h
	$(CC) $(CFLAGS) -c main.c

filesystem.o: libs/filesystem/filesystem.c libs/filesystem/filesystem.h libs/user/user.h
	$(CC) $(CFLAGS) -c libs/filesystem/filesystem.c -o filesystem.o

user.o: libs/user/user.c libs/user/user.h
	$(CC) $(CFLAGS) -c libs/user/user.c -o user.o

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -f *.o filesystem.o user.o utils.o $(BIN_DIR)/main