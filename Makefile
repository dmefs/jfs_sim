CC=	gcc
CXX=g++
OBJ_DIR=obj
IMR_DIR=IMRSimulator/src
IMRS=$(wildcard $(IMR_DIR)/*.c)
SRC_DIR=src
SRC := $(wildcard $(SRC_DIR)/*.c)

IMR_OBJS= IMRSimulator/src/lba.c IMRSimulator/src/pba.c IMRSimulator/src/batch.c IMRSimulator/src/chs.c IMRSimulator/src/record_op.c IMRSimulator/src/rw.c
TOP_BUFFER_OBJS= IMRSimulator/src/top_buffer.c IMRSimulator/src/scp.c
VG_OBJS = IMRSimulator/src/fid_table.c IMRSimulator/src/virtual_groups.c IMRSimulator/src/dump.c
JFS_OBJS= src/command_table.c src/jfs.c

CPPFLAGS=-std=c++11 -Wfatal-errors -Wall 
LDFLAGS= -lgtest -lpthread
CFLAGS=-Wfatal-errors -Wall 
INCLUDE_FLAGS=-IIMRSimulator/include -Isrc

ut_main: dirs
	$(CXX) $(CPPFLAGS) test/ut_main.cpp $(SRC) $(IMRS) -o bin/ut_main $(LDFLAGS)

imr: $(OBJS) dirs
	$(CC) $(CFLAGS) -DNATIVE test/main.c -o bin/jfs $(JFS_OBJS) $(IMR_OBJS) $(INCLUDE_FLAGS) -g

zalloc: $(OBJS) dirs
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -DZALLOC test/main.c -o bin/jfs $(OBJS) $(JFS_OBJS) $(IMR_OBJS) -g

top_buffer: $(OBJS) $(TOP_BUFFER_OBJS)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -DTOP_BUFFER test/main.c -o bin/jfs $(JFS_OBJS) $(IMR_OBJS) $(OBJS) $(TOP_BUFFER_OBJS) -g

vg: $(OBJS) dirs
	$(CC) $(CFLAGS) -DVIRTUAL_GROUPS test/main.c -o bin/jfs $(JFS_OBJS) $(IMR_OBJS) $(VG_OBJS) $(INCLUDE_FLAGS)

test: vg
	./bin/jfs -i instructions/1m_1024 -s 2

test1g: zalloc
	./bin/jfs -i instructions/1m_1024_333 -s 2

test100g: imr
	./bin/jfs -i instructions/1g_100 -s 110

ut_test: ut_main
	./bin/ut_main

dirs:
	mkdir -p bin

clean:
	rm -f bin/*