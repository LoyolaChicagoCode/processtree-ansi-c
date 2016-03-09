CC=gcc
CXX=g++
CFLAGS=-g -ansi -pedantic -D_POSIX_C_SOURCE -DMAX_PROCS=1000000
LDLIBS=-lstdc++

fakeps: fakeps.o

clean:
	-rm -f fakeps.o fakeps

