CC=gcc
CFLAGS=-ansi -pedantic -D_POSIX_C_SOURCE=3

processtree: processtree.o

processtree.o: processtree.c processtree.h

clean:
	-rm -f processtree.o processtree

