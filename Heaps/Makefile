CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -o4 -I .
TARGET = heap_test.x

all: $(TARGET)

$(TARGET): heap.cc
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: clean all

clean:
	rm $(TARGET)

heap.cc: ./heap.h
