CC=gcc
CFLAGS=-g -ansi -pedantic -D_POSIX_C_SOURCE -DMAX_PROCS=1000000

processtree: processtree.o

processtree.o: processtree.c processtree.h

clean:
	-rm -f processtree.o processtree

