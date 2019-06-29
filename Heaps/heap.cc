#include <iostream>
#include <functional>
#include <chrono>
//#include <sys/time.h>

#include "heap.h"
// maximum value for the heap elements
#define MAX_VALUE 25

/**
  * This function returns the number of second elapsed since
  * the start of the Unix epoch on 1 January, 1970
  *//*
double seconds() {
    struct timeval tmp;
    double sec;
    gettimeofday(&tmp, (struct timezone*)0);
    sec = tmp.tv_sec * 1000000.0 + tmp.tv_usec;
    return sec;
}*/

int main() {
    // in the following, tests will be made using the functionalities of the
    // <chrono> header. I am aware that this is a break of the no-STL rule,
    // but the clock used by C++ is the highest-resolution clock in the world,
    // and the functions of the <time.h> header provided sometime bogus results

    std::cout << "TESTING THE IMPLEMENTATION OF HEAPIFY:" << std::endl;
    // random seed
    srand(0);
    // loop over the set of dimensions to investigate
    for (std::size_t dim : {1000, 5000, 10000, 50000, 100000, 500000, 1000000}) {
        int test[dim];
        // initialize array of random values
        for (std::size_t i=0; i < dim; ++i) {
            test[i] = rand()%(2*MAX_VALUE)-MAX_VALUE;
        }
        { //new scope to make sure the destructor of BinaryHeap gets called before the next iteration,
          // to avoid subtle memory leaks
            // build a max-heap of int, having elements in 'test' and dimensions 'dim'
            BinaryHeap<int, std::greater<int>> h{test, dim};
            h[0] = 0.0;    // break heap property, so that a call to heapify on the root makes sense
            // measure time
            //double start = seconds();
            auto start = std::chrono::high_resolution_clock::now();
            h.heapify(0);
            auto end = std::chrono::high_resolution_clock::now();
            //double end = seconds();
            // print result
            std::cout << "Size: " << dim << std::endl;
            std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << std::endl;
        }
    }
    return 0;
}
