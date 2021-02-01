CC=	gcc
CXX=g++
OBJ_DIR=obj
IMR_DIR=IMRSimulator/src
IMRS=$(wildcard $(IMR_DIR)/*.c)
SRC_DIR=src
SRC := $(wildcard $(SRC_DIR)/*.c)

CPPFLAGS=-std=c++11 -Wfatal-errors -Wall 
IMRFLAGS=-DZALLOC_IMR -DTOP_BUFFER
LDFLAGS= -lgtest -lpthread

ut_main: dirs
	$(CXX) $(CPPFLAGS) $(IMRFLAGS) test/ut_main.cpp $(SRC) $(IMRS) -o bin/ut_main $(LDFLAGS)

ut_test: ut_main
	./bin/ut_main

dirs:
	mkdir -p bin

clean:
	rm -f bin/*