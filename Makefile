# Makefile

CC = gcc
CFLAGS = -Wall -Ilibs/filesystem -Ilibs/user -Ilibs/fcb -Ilibs/permission
BIN_DIR = bin

OBJS = main.o filesystem.o fcb.o user.o

all: $(BIN_DIR)/main

$(BIN_DIR)/main: $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/main $(OBJS)

main.o: main.c libs/filesystem/filesystem.h libs/user/user.h
	$(CC) $(CFLAGS) -c main.c

filesystem.o: libs/filesystem/filesystem.c libs/filesystem/filesystem.h libs/fcb/fcb.h
	$(CC) $(CFLAGS) -c libs/filesystem/filesystem.c -o filesystem.o

fcb.o: libs/fcb/fcb.c libs/fcb/fcb.h libs/permission/permission.h
	$(CC) $(CFLAGS) -c libs/fcb/fcb.c -o fcb.o

user.o: libs/user/user.c libs/user/user.h
	$(CC) $(CFLAGS) -c libs/user/user.c -o user.o

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -f *.o $(BIN_DIR)/main
