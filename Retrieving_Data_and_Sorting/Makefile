CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I ../Heaps
TARGET = sorting_test.x

all: $(TARGET)

$(TARGET): sorting_and_searching.cc
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY: clean all

clean:
	rm $(TARGET)

sorting_and_searching.cc: ../Heaps/heap.h
