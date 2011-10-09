CC=gcc
CFLAGS=-g -ansi -pedantic -D_POSIX_C_SOURCE

processtree: processtree.o

processtree.o: processtree.c processtree.h

clean:
	-rm -f processtree.o processtree

