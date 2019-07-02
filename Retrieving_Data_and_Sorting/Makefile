CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I ../Heaps -I .

COMPARISON_TARGET = comparison_sort.x
COMPARISON_SOURCE = comparison_sort.cc

LINEAR_TARGET = linear.x
LINEAR_SOURCE = linear_time_sort.cc

SELECT_TARGET = select.x
SELECT_SOURCE = select.cc

all: $(COMPARISON_TARGET) $(LINEAR_TARGET) $(SELECT_TARGET)

$(COMPARISON_TARGET): $(COMPARISON_SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(LINEAR_TARGET): $(LINEAR_SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SELECT_TARGET): $(SELECT_SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: clean all

clean:
	rm $(COMPARISON_TARGET) $(LINEAR_TARGET) $(SELECT_TARGET)

comparison_sort.cc: ../Heaps/heap.h
select.cc: ./sort_utils.h
linear_time_sort.cc: ./sort_utils.h
