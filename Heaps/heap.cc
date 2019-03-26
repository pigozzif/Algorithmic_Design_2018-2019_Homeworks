#include <iostream>
#include <functional>
#include <chrono>
#include "heap.h"

#define MAX_VALUE 25

int main() {
    std::cout << "TESTING THE IMPLEMENTATION OF HEAPIFY:" << std::endl;
    srand(0);
    // loop over the set of dimensions to investigate
    for (std::size_t dim : {10, 50, 100, 500, 1000, 5000, 10000, 50000, 100000}) {
        int test[dim];
        // initialize array of random values
        for (std::size_t i=0; i < dim; ++i) {
            test[i] = rand()%(2*MAX_VALUE)-MAX_VALUE;
        }
        {//new scope to make sure the destructor of BinaryHeap gets called before the next iteration
        BinaryHeap<int,std::greater<int>> h{test, dim};
        h[0] = 0.0;    // break heap property
        // measure time
        auto start = std::chrono::high_resolution_clock::now();
        h.heapify(0);
        auto end = std::chrono::high_resolution_clock::now();
        // print result
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << std::endl;
      }
    }
    return 0;
}
