CC = gcc
CCFLAGS = -std=gnu99 -O2 -Wall -g -lev -pthread


all: init toyws

init:
	mkdir -p bin

toyws: src/*
	$(CC) $(CCFLAGS) $^ -o bin/$@.bin

#rio:src/rio.c src/rio.h
#	$(CC) -shared -fPIC $(CCFLAGS) $^ -o bin/$@

run: bin/toyws
	bin/toyws 8888

test:
	curl -XPOST -H "Content-Type: text/html"  -d "Hello":"World" 127.0.0.1:8888

clean: 
	rm bin/toyws.bin 
