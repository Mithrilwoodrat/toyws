CC = gcc
CCFLAGS = -std=gnu99 -O2 -Wall -g -pthread


all: init toyws

init:
	mkdir -p bin

toyws: toyws.c toyws.h rio.h rio.c
	$(CC) $(CCFLAGS) $^ -o bin/$@

#rio:rio.c rio.h
#	$(CC) -shared -fPIC $(CCFLAGS) $^ -o $@

clean: 
	rm bin/toyws bin/rio
