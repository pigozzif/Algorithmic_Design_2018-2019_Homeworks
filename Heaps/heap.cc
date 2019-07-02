#include <iostream>
#include <chrono>

#include "heap.h"
// maximum value for the heap elements
#define MAX_VALUE 25

/**
  * Function object to play the role of 'std::greater' of the STL.
  * To be used to create a max-heap
  */
template<class T>
struct CompareItems {
    // default constructor
    CompareItems() = default;
    // overload of operator(); returns true if a > b, false otherwise
    bool operator()(const T a, const T b) {
        return a > b;
    }
};

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
            BinaryHeap<int, CompareItems<int>> h{test, dim};
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
