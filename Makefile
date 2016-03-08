CC=gcc
CFLAGS=-g -ansi -pedantic -D_POSIX_C_SOURCE -DMAX_PROCS=1000000

processtree: processtree.o

processtree.o: processtree.c processtree.h

clean:
	-rm -f processtree.o processtree

ps: processtree
	ps -ef | time ./processtree

psfake: processtree
	python fakeps.py 10000 | time ./processtree

