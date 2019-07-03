#include <iostream>
#include <cmath>
#include <chrono>

#define MAX_VALUE 25  // maximum value to be used in the tests

/**
  * Radix sort; takes an array to sort 'A', max number of digits 'd', and the length of the array
  * 'n'. Internally uses counting sort modified to work on one digit per time
  */
void radix_sort(int* A, const int d, const std::size_t n) {  // d is the max number of digits
    // C is the counter array, B will store intermediate outputs
    int* C{new int[10]};
    int* B{new int[n]};
    // for each digit j from the most significant to the least
    for (int j=pow(10, d - 1); j >=1; j*=10) {
        // perform counting sort according to digit j
        for (std::size_t i=0; i < 10; ++i) {
            C[i] = 0;
        }
        for (std::size_t i=0; i < n; ++i) {
            ++C[(A[i] / j) % 10];
        }
        for (std::size_t i=1; i < 10; ++i) {
            C[i] += C[i - 1];
        }
        for (std::size_t i=n; i >= 1; --i) {
            int v{A[i-1]};
            B[C[(A[i-1] / j) % 10] - 1] = v;
            --C[(A[i-1] / j) % 10];
        }
        // save intermediate result into A
        for (std::size_t i=0; i < n; ++i) {
            A[i] = B[i];
        }
    }
    // deallocate
    delete[] B;
    delete[] C;
}

/**
  * Main function
  */
int main() {

    // the following block of code tests the execution time of Radix Sort

    std::cout << "TESTING THE IMPLEMENTATION OF RADIX SORT" << std::endl;
    for (std::size_t dim : {100, 250, 500, 750, 1000, 1250, 2500, 3000, 4000, 5000, 10000, 100000}) {
        int test[dim];
        // initialize array of random values
        for (std::size_t i=0; i < dim; ++i) {
            test[i] = rand()%(2*MAX_VALUE)-MAX_VALUE;
        }
        // measure time
        auto start = std::chrono::high_resolution_clock::now();
        radix_sort(test, 2, dim);
        auto end = std::chrono::high_resolution_clock::now();
        // print result
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    return 0;
}
