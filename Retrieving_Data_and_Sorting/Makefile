CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I ../Heaps -I .

COMPARISON_TARGET = comparison_sort.x
COMPARISON_SOURCE = comparison_sort.cc

COUNT_TARGET = count.x
COUNT_SOURCE = counting_sort.cc

RADIX_TARGET = radix.x
RADIX_SOURCE = radix_sort.cc

BUCKET_TARGET = bucket.x
BUCKET_SOURCE = bucket_sort.cc

SELECT_TARGET = select.x
SELECT_SOURCE = select.cc

all: $(COMPARISON_TARGET) $(COUNT_TARGET) $(SELECT_TARGET) $(RADIX_TARGET) $(BUCKET_TARGET)

$(COMPARISON_TARGET): $(COMPARISON_SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(COUNT_TARGET): $(COUNT_SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(RADIX_TARGET): $(RADIX_SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUCKET_TARGET): $(BUCKET_SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SELECT_TARGET): $(SELECT_SOURCE)
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: clean all

clean:
	rm $(COMPARISON_TARGET) $(BUCKET_TARGET) $(SELECT_TARGET) $(COUNT_TARGET) $(RADIX_TARGET)

comparison_sort.cc: ../Heaps/heap.h
select.cc: ./sort_utils.h
bucket_sort.cc: ./sort_utils.h
