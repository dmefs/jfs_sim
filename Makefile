CC=	gcc
CXX=g++
OBJ_DIR=obj
IMR_DIR=IMRSimulator/src
IMRS=$(wildcard $(IMR_DIR)/*.c)
SRC_DIR=src
SRC := $(wildcard $(SRC_DIR)/*.c)

IMR_OBJS= IMRSimulator/src/lba.c IMRSimulator/src/pba.c IMRSimulator/src/batch.c IMRSimulator/src/chs.c IMRSimulator/src/record_op.c IMRSimulator/src/rw.c
JFS_OBJS= src/command_table.c src/jfs.c

CPPFLAGS=-std=c++11 -Wfatal-errors -Wall 
IMRFLAGS=-DZALLOC_IMR -DTOP_BUFFER
LDFLAGS= -lgtest -lpthread
CFLAGS=-Wfatal-errors -Wall 

ut_main: dirs
	$(CXX) $(CPPFLAGS) $(IMRFLAGS) test/ut_main.cpp $(SRC) $(IMRS) -o bin/ut_main $(LDFLAGS)

imr: $(OBJS)
	$(CC) $(CFLAGS) test/main.c -o bin/jfs $(JFS_OBJS) $(IMR_OBJS) -IIMRSimulator/src -Isrc -g

test: imr
	./bin/jfs -i instructions/10m_100 -s 2

ut_test: ut_main
	./bin/ut_main

dirs:
	mkdir -p bin

clean:
	rm -f bin/*