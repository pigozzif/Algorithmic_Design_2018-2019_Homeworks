CC = gcc
CCFLAGS = -Wall -Wextra -o4 -I .
SRC = $(wildcard *.c)
TARGET = strassen_test.x

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CCFLAGS) -o $@ $^

.PHONY: clean all

clean:
	rm $(TARGET)

matrix.c: ./matrix.h
strassen.c: ./matrix.h ./strassen.h
main.c: ./matrix.h ./strassen.h
