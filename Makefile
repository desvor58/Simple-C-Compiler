CC=gcc
CCFLAGS=-Iinclude
SRC=src/*.c
OUT=bin/scc.exe

all:
	$(CC) $(SRC) -o $(OUT) $(CCFLAGS)