#
# Makefile
#

LIB = ncurses
CC = gcc
++ = g++

all: nclient nserver

nclient: nclient.c msock.o msock.h
	$(CC) -o nclient nclient.c msock.o

nserver: nserver.cpp msock.o msock.h
	$(++) -o nserver nserver.cpp msock.o

msock: msock.c msock.h
	$(CC) -c msock.c

clean:
	/bin/rm -rf nclient *.o *~ core
	/bin/rm -rf nserver *.o *~ core


