CC = gcc
CCFLAGS = -o4 -Wall -Wextra -I .
SRC = $(wildcard *.c)
TARGET = chain_test.x

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CCFLAGS) -o $@ $^

.PHONY: clean all

clean:
	rm $(TARGET)

$(SRC): ./matrix.h
