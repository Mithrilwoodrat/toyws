CC = gcc
CCFLAGS = -std=gnu99 -O2 -Wall -g


all: init toyws

init:
	mkdir -p bin

toyws: toyws.c toyws.h rio.o
	$(CC) $(CCFLAGS) $^ -o bin/$@

rio:rio.c rio.h
	$(CC) $(CCFLAGS) $^ -o bin/$@

clean: 
	rm bin/toyws
