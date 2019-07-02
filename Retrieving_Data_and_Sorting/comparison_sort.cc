#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <cmath>

#include "heap.h"
#include "sort_utils.h"
// maximum value to be used in the tests
#define MAX_VALUE 25

/**
  * Dichotomic search. Takes the length of array 'A', 'r', and a candidate element
  * 'a'. Returns the index position of a in A, -1 if not found
  */
template<class T>
int di_find(T* A, const T a, int r) {
    // 'r' will be the right index and 'l' the left one
    int l{0};
    int m{};
    // as long as we consider a non-negative length portion
    while (r >= l) {
        // let 'm' be the midpoint
        m = (l + r) / 2;
        // if A[m] is the candidate, return
        if (A[m] == a) {
            return m;
        }
        // if the candidate is smaller, continue search to the right of m
        else if (A[m] < a) {
            l = m + 1;
        }
        // else continue search to the left of m
        else {
            r = m - 1;
        }
    }
    // not found
    return -1;
}

/**
  * Quicksort; takes an array to sort 'A', lower and upper bounds' indexes 'l' and 'r',
  * and a pointer to a function 'func' used to choose the pivot. Sorts in-place and avoids
  * the tail recursion. 'func' has a lambda function as default argument returning always
  * the lower index of the current subarray, corresponding to the
  */
template<class T>
void quicksort(T* A, int l, int r, int (*func)(const int, const int)=[](const int l, const int r) {(void)r; return l;}) {
    // as long as the current subarray has length > 1
    while (l < r) {
        // apply 'partition' to choose a pivot
        int pivot = partition(A, l, r-1, func(l, r));
        // recursively sort the left portion
        quicksort(A, l, pivot, func);
        // now focus on the right portion
        l = pivot + 1;
    }
}

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

/**
  * Heapsort sorting algorithm. Takes an array to sort 'A' and its length 'n'. Sorts in-place by building a max-heap,
  * defined in the heap.h header. Reassigning the pointer to the array allows us to implement it in-place.
  * The template is on the type of the array to sort
  */
template<class T>
void heapsort(T* A, const std::size_t n) {
    // build a max-heap, the root will be the maximum
    BinaryHeap<T, CompareItems<T>> h{A, n, true};   //CompareItems allows us to represent a max-heap
    // for each element - 1
    for (std::size_t i=n-1; i >= 1; --i) {
        // swap the i-th element with the root
        h.swap(i, 0);
        // remove the last leaf
        --h.size;
        // fix the max-heap
        h.heapify(0);
    }
    // since the 'data' member of the heap points to the same
    // memory location as 'A', the array has been sorted
}

/**
  * Main function
  */
int main() {
    //int test[10] = {13, 5, 7, 5, 12, 4, 9, 11, 4, 1};
    //int test[24] = {13, 5, 7, 2, 7, 4, 1, 11, 6, 0, 10, 15, -1, 8, -2, 3, 14, 12, -9, -5, 9, -6, 16, -4};
    //double test[10] = {0.23, 0.60, 0.20, 0.73, 0.97, 0.34, 0.01, 0.50, 0.78, 0.44};
    //bucket_sort(test, 10, 5);
    //int k = partition(test, 0, 9, 2);
    //std::cout << partition_for_select(test, 0, k, k) << std::endl;
    //std::cout << select(test, 6, 0, 9) << "###" << std::endl;
    //for (int i=0; i < 10; ++i) std::cout << test[i] << " ";


    // the following block of code proves by testing that Insertion Sort takes time O(n^2) in the worst case

    std::cout << "TESTING THE IMPLEMENTATION OF INSERTION_SORT IN THE WORST CASE:" << std::endl;
    for (std::size_t dim : {100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000}) {
        int test[dim];
        // initialize array of reverse-sorted values, which is the worst-case for insertion sort
        int count = 0;
        for (int i=dim-1; i >= 0; --i) {
            test[i] = count;
            ++count;
        }
        // measure time
        auto start = std::chrono::high_resolution_clock::now();
        insertion_sort(test, dim);
        auto end = std::chrono::high_resolution_clock::now();
        // print result
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    // the following block of code proves by testing that Insertion Sort takes time O(n) in the best case

    std::cout << "TESTING THE IMPLEMENTATION OF INSERTION_SORT IN THE BEST CASE:" << std::endl;
    for (std::size_t dim : {100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000}) {
        int test[dim];
        // initialize array of already-sorted values, which is the best case for insertion sort
        for (std::size_t i=0; i < dim; ++i) {
            test[i] = i;
        }
        // measure time
        auto start = std::chrono::high_resolution_clock::now();
        insertion_sort(test, dim);
        auto end = std::chrono::high_resolution_clock::now();
        // print result
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    // the following block of code proves by testing that Quicksort takes time O(n^2) in the worst case

    int (*choose_pivot) (const int, const int);   // function pointer to be passed to quicksort in order to select the pivot
    choose_pivot = [] (const int l, const int r) -> int {(void)r; return l;};  // let it point to a lambda function that always returns the first element,
    // which is the worst case for Quicksort (the last element is a valid choice as well) assuming the array is sorted in descending or ascending order
    std::cout << "TESTING THE IMPLEMENTATION OF QUICKSORT IN THE WORST CASE:" << std::endl;
    for (std::size_t dim : {100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000}) {
        int test[dim];
        // initialize already sorted array
        for (std::size_t i=0; i < dim; ++i) {
            test[i] = i;
        }
        // measure time
        auto start = std::chrono::high_resolution_clock::now();
        quicksort<int>(test, 0, dim, choose_pivot);
        auto end = std::chrono::high_resolution_clock::now();
        // print result
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    // the following block of code proves by testing that Quicksort takes time O(nlogn) in the best case

    choose_pivot = [] (const int l, const int r) -> int {return (l + r) / 2;};  // let it point to a lambda function that always returns the midpoint,
    // which is the best case for Quicksort assuming the array is sorted in descending or ascending order
    std::cout << "TESTING THE IMPLEMENTATION OF QUICKSORT IN THE BEST CASE:" << std::endl;
    for (std::size_t dim : {100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000}) {
        int test[dim];
        // initialize already sorted array
        for (std::size_t i=0; i < dim; ++i) {
            test[i] = i;
        }
        // measure time
        auto start = std::chrono::high_resolution_clock::now();
        quicksort<int>(test, 0, dim, choose_pivot);
        auto end = std::chrono::high_resolution_clock::now();
        // print result
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    // the following block of code tests the execution time of Heapsort

    std::cout << "TESTING THE IMPLEMENTATION OF HEAPSORT:" << std::endl;
    for (std::size_t dim : {10, 50, 100, 500, 1000, 5000, 10000, 50000, 100000}) {
        int test[dim];
        // initialize array of random values
        for (std::size_t i=0; i < dim; ++i) {
            test[i] = rand()%(2*MAX_VALUE)-MAX_VALUE;
        }
        // measure time
        auto start = std::chrono::high_resolution_clock::now();
        heapsort(test, dim);
        auto end = std::chrono::high_resolution_clock::now();
        // print result
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    return 0;
}
