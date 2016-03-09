CC=gcc
CXX=g++
CFLAGS=-g -ansi -pedantic -D_POSIX_C_SOURCE -DMAX_PROCS=1000000
LDLIBS=-lstdc++

fakeps: fakeps.o

fakeps.o: fakeps.cc

clean:
	-rm -f fakeps.o fakeps

