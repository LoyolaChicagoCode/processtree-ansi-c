CC=gcc
CFLAGS=-g -ansi -pedantic -D_POSIX_C_SOURCE -DMAX_PROCS=1000000

fakeps-simple: fakeps-simple.o

clean:
	-rm -f fakeps-simple.o fakeps-simple

