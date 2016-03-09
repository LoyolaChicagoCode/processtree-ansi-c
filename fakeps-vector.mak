CC=gcc
CXX=g++
CFLAGS=-g -ansi -pedantic -D_POSIX_C_SOURCE -DMAX_PROCS=1000000
LDLIBS=-lstdc++

fakeps-vector: fakeps-vector.o

clean:
	-rm -f fakeps-vector.o fakeps-vector

