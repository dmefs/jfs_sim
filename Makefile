CC=	gcc
CXX=g++
OBJ_DIR=obj
IMR_DIR=IMRSimulator/src
IMRS=$(wildcard $(IMR_DIR)/*.c)
SRC_DIR=src
SRC := $(wildcard $(SRC_DIR)/*.c)

IMR_OBJS= IMRSimulator/src/lba.c IMRSimulator/src/pba.c IMRSimulator/src/batch.c IMRSimulator/src/chs.c IMRSimulator/src/record_op.c IMRSimulator/src/rw.c
TOP_BUFFER_OBJS= IMRSimulator/src/top_buffer.c IMRSimulator/src/scp.c
BLOCK_SWAP_OBJS= IMRSimulator/src/block_swap.c
VG_OBJS = IMRSimulator/src/fid_table.c IMRSimulator/src/virtual_groups.c IMRSimulator/src/dump.c 
VG_HISTORY_OBJS = IMRSimulator/src/ring_buffer.c
JFS_OBJS= src/command_table.c src/jfs.c

CPPFLAGS=-std=c++11 -Wfatal-errors -Wall
LDFLAGS= -lgtest -lpthread
CFLAGS=-Wfatal-errors -Wall
INCLUDE_FLAGS=-IIMRSimulator/include -Isrc
IMR_FLAGS=-DJFS

ifdef DEBUG
CFLAGS += -DCONFIG_DEBUG_LIST
VG_OBJS += IMRSimulator/src/list_debug.c
endif

all: cmr native blockswap blockswap_virtual vg vg_reserved vg_history 
ut_main: dirs
	$(CXX) $(CPPFLAGS) test/ut_main.cpp $(SRC) $(IMRS) -o bin/ut_main $(LDFLAGS)

cmr: $(OBJS) dirs
	$(CC) $(CFLAGS) -DCMR test/main.c -o bin/cmr $(JFS_OBJS) $(IMR_OBJS) $(INCLUDE_FLAGS) $(IMR_FLAGS)

native: $(OBJS) dirs
	$(CC) $(CFLAGS) -DNATIVE test/main.c -o bin/native $(JFS_OBJS) $(IMR_OBJS) $(INCLUDE_FLAGS) $(IMR_FLAGS)

zalloc: $(OBJS) dirs
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -DZALLOC test/main.c -o bin/zalloc $(OBJS) $(JFS_OBJS) $(IMR_OBJS) $(IMR_FLAGS)

topbuffer: $(OBJS) $(TOP_BUFFER_OBJS) dirs
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -DTOP_BUFFER test/main.c -o bin/topbuffer $(JFS_OBJS) $(IMR_OBJS) $(OBJS) $(TOP_BUFFER_OBJS) $(IMR_FLAGS)

blockswap: $(OBJS) $(TOP_BUFFER_OBJS) $(BLOCK_SWAP_OBJS) dirs
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -DBLOCK_SWAP test/main.c -o bin/blockswap $(JFS_OBJS) $(IMR_OBJS) $(OBJS) $(TOP_BUFFER_OBJS) $(BLOCK_SWAP_OBJS) $(IMR_FLAGS)

blockswap_virtual: $(OBJS) $(TOP_BUFFER_OBJS) $(BLOCK_SWAP_OBJS) dirs
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -DBLOCK_SWAP -DZALLOC_JOURNAL_VIRTUAL test/main.c -o bin/blockswap_virtual $(JFS_OBJS) $(IMR_OBJS) $(OBJS) $(TOP_BUFFER_OBJS) $(BLOCK_SWAP_OBJS) $(IMR_FLAGS)

vg: $(OBJS) dirs $(OBJS) $(VG_OBJS) dirs
	$(CC) $(CFLAGS) -DVIRTUAL_GROUPS test/main.c -o bin/vg $(JFS_OBJS) $(IMR_OBJS) $(VG_OBJS) $(INCLUDE_FLAGS) $(IMR_FLAGS)

vg_reserved: $(OBJS) dirs $(OBJS) $(VG_OBJS) dirs
	$(CC) $(CFLAGS) -DVIRTUAL_GROUPS -DRESERVED_END_TRACK test/main.c -o bin/$@ $(JFS_OBJS) $(IMR_OBJS) $(VG_OBJS) $(INCLUDE_FLAGS) $(IMR_FLAGS)

vg_history: $(OBJS) dirs $(OBJS) $(VG_OBJS) $(VG_HISTORY_OBJS) dirs
	$(CC) $(CFLAGS) -DVIRTUAL_GROUPS -DATTR_HISTORY test/main.c -o bin/$@ $(JFS_OBJS) $(IMR_OBJS) $(VG_OBJS) $(VG_HISTORY_OBJS) $(INCLUDE_FLAGS) $(IMR_FLAGS)

test1g: top_buffer
	./bin/jfs -i instructions/1m_1024_333 -s 2

test100g: imr
	./bin/jfs -i instructions/1g_100 -s 110

SIZE=2
TRACE=1m_1024_333
EXEC_FILE="cmr native blockswap blockswap_virtual vg vg_reserved vg_history"
run_trace:
	@./run_trace.sh
	@python3 csv_to_excel.py
testall: cmr native blockswap blockswap_virtual vg vg_reserved vg_history
	./bin/native -s $(SIZE) -i instructions/$(TRACE)
	./bin/zalloc -s $(SIZE) -i instructions/$(TRACE)
	./bin/topbuffer -s $(SIZE) -i instructions/$(TRACE)
	./bin/blockswap -s $(SIZE) -i instructions/$(TRACE)
	./bin/vg -s $(SIZE) -i instructions/$(TRACE)

ut_test: ut_main
	./bin/ut_main

dirs:
	mkdir -p bin

clean:
	rm -f bin/* *.csv *.xlsx