#include <iostream>
#include <chrono>
#include "heap.h"

#define MAX_VALUE 25

/* The main function includes time tests on the free sorting routines here implemented.
 * It turns out that, on average, insertion sort is considerably slower, and its execution
 * can grow quadratically with respect to the problem size. On the other side, quicksort tended
 * to perform very well on small problem instances, but was beaten by heapsort on large arrays.
 * In fatc, we now that the latter grows at most as nlogn, whereas quicksort does so only on average.
 */

// swap helper function

template<class T>
void swap(T* A, int i, int j) {
    T temp{A[i]};
    A[i] = A[j];
    A[j] = std::move(temp);
}

// dichotomic search

template<class T>
int di_find(T* A, T a, int r) {
    int l{0};
    int m{};
    while (r >= l) {
      m = (l + r) / 2;
      if (A[m] == a) return m;
      else if (A[m] < a) l = m + 1;
      else r = m - 1;
    }
    return -1;
}

// insertion sort

template<class T>
void insertion_sort(T* A, int n) {
    for (int i=1; i < n; ++i) {
        int j{i};
        while (j > 0 && A[j] < A[j - 1]) {
            swap(A, j-1, j);
            --j;
        }
    }
}

// partition routine

template<class T>
int partition(T* A, int l, int r, int p) {
    T pivot{A[p]};
    A[p] = A[l];
    A[l] = pivot;
    int i{l};
    for (int j=i+1; j < r; ++j) {
        if (A[j] < pivot) {
          swap(A, i, j);
          ++i;
        }
    }
    return i;
}

// quicksort

template<class T>
void quicksort(T* A, int l, int r) {
    while (l < r) {
        int p = partition(A, l, r, l);
        quicksort(A, l, p - 1);
        l = p + 1;
    }
}

// heapsort

template<class T>
void heapsort(T* A, std::size_t n) {
    BinaryHeap<T,std::greater<T>> h{A, n, true};   //std::greater allows us to represent a maxheap
    for (std::size_t i=n-1; i >= 1; --i) {
        h.swap(i, 0);
        --h.size;
        ++h.free_slots;
        h.heapify(0);
    }
    A = std::move(h.data);
}

int main() {
    std::cout << "TESTING THE IMPLEMENTATION OF INSERTION_SORT:" << std::endl;
    for (std::size_t dim : {10, 50, 100, 500, 1000, 5000, 10000, 50000, 100000}) {
        int test[dim];
        // initialize array of random values
        for (int i=0; i < dim; ++i) {
            test[i] = rand()%(2*MAX_VALUE)-MAX_VALUE;
        }
        auto start = std::chrono::high_resolution_clock::now();
        insertion_sort(test, dim);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    std::cout << "TESTING THE IMPLEMENTATION OF QUICKSORT:" << std::endl;
    for (std::size_t dim : {10, 50, 100, 500, 1000, 5000, 10000, 50000, 100000}) {
        int test[dim];
        // initialize array of random values
        for (int i=0; i < dim; ++i) {
            test[i] = rand()%(2*MAX_VALUE)-MAX_VALUE;
        }
        auto start = std::chrono::high_resolution_clock::now();
        quicksort(test, 0, dim);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    std::cout << "TESTING THE IMPLEMENTATION OF HEAPSORT:" << std::endl;
    for (std::size_t dim : {10, 50, 100, 500, 1000, 5000, 10000, 50000, 100000}) {
        int test[dim];
        // initialize array of random values
        for (int i=0; i < dim; ++i) {
            test[i] = rand()%(2*MAX_VALUE)-MAX_VALUE;
        }
        auto start = std::chrono::high_resolution_clock::now();
        heapsort(test, dim);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }
    
    return 0;
}
