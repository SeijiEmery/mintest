
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -I ..

all: example

example: example.c
	$(CC) $(CFLAGS) -o $@ $<

run: example
	./example

clean:
	rm -rf example
