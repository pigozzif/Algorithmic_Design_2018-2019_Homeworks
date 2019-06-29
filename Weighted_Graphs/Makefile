CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -o4 -I . -I ../Heaps

SRC = dijkstra.cc
TARGET = dijkstra.x

all: $(TARGET)

$(TARGET): $(SRC)
	  $(CXX) $(CXXFLAGS) $< -o $@

.PHONY: all clean

clean:
	  rm $(TARGET)

$(SRC): ./graph_utilities.h ../Heaps/heap.h
