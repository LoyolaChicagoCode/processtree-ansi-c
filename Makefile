CC=gcc
CFLAGS=-g -ansi -pedantic -D_POSIX_C_SOURCE

processtree: processtree.o

processtree.o: processtree.c processtree.h

clean:
	-rm -f processtree.o processtree

ps: processtree
	ps -ef | ./processtree

psfake: processtree
	python fakeps.py 10000 | ./processtree

