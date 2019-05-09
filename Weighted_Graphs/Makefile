CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I . -I ../Heaps

SRC = dijkstra.cc
TARGET = dijkstra.x

all: $(TARGET)

$(TARGET): $(SRC)
	  $(CXX) $(CXXFLAGS) $< -o $@

.PHONY: all clean

clean:
	  rm $(TARGET)

$(SRC): ./queue.h ../Heaps/heap.h
