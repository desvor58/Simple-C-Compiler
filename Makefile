CC=gcc
CCFLAGS=-Iinclude
SRC=src/*.c
OUT=bin/prcc.exe

all:
	$(CC) $(SRC) -o $(OUT) $(CCFLAGS)