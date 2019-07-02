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
  * Counting sort; takes an array to sort 'A', the destination array 'B', lower and upper
  * bounds for the domain 'k1' and 'k2', and the length of the array 'n'
  */
template<class T>
void counting_sort(T* A, T* B, const std::size_t k1, const std::size_t k2, const std::size_t n) {
    std::size_t k{k2 - k1 + 1};  // length of our domain
    // allocate a new array
    int* C{new int[k]};
    // initialize the array to 0
    for (std::size_t j=0; j < k; ++j) {
        C[j] = 0;
    }
    // iterate over A and make sure C[j] is the number of j in A
    for (std::size_t i=0; i < n; ++i) {
        ++C[A[i] - k1];
    }
    // iterate over C and make sure C[j] is the number of A's values <= j
    for (std::size_t j=1; j < k; ++j) {
        C[j] += C[j - 1];
    }
    // iterate backward on A and use C to place the elements in their correct position in B
    for (std::size_t i=n; i >= 1; --i) {
        T v{A[i - 1]};    // we use i-1 because otherwise the condition is always true for std::size_t
        B[C[v - k1] - 1] = v;
        --C[v - k1];
    }
    // deallocate
    delete[] C;
}

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
  * Dynamic array implementation. It stores an array of data and provides a reallocation mechanism for better
  * memory management, so that it can gradually grow in size. It is intended to be used inside 'bucket_sort' for
  * the buckets
  */
template<class T>
class List {
    std::size_t free_slots;  // free space
    std::size_t _size;  // number of elements
    T* data;  // array of data to store
  public:
    // default constructor, initializes a 1-dimensional array
    List() : free_slots{0}, _size{0}, data{new T[1]} {}
    // returns  the size
    std::size_t size() const noexcept {
        return _size;
    }
    // inserts a new value
    void append(const T& value) {
        // check if there is free space, if not reallocate data
        check_and_alloc();
        // append value and update members
        data[_size] = value;
        ++_size;
        --free_slots;
    }
    // sorts the bucket. Uses insertion_sort
    void sort() noexcept {
        insertion_sort(data, _size);
    }
    // overload of operator[]
    T& operator[](const std::size_t i) const noexcept {
        return data[i];
    }
    // Destructor
    ~List() {
        delete[] data;
    }

  private:
    // helper function to dynamically reallocate the array
    void check_and_alloc() {
        // if still free space in the data, do nothing
        if (free_slots != 0) {
            return;
        }
        // temporary array with double the size
        // we have assumed the distribution over buckets is uniform, but the optimal choice for how much
        // to reallocate actually depends on the relation between the size of the array n and the number
        // of buckets k. Here we will assume double each time
        T* new_data{new T[_size * 2]};
        // copy the elements one by one
        for (std::size_t i=0; i < _size; ++i) {
            new_data[i] = data[i];
        }
        data = new_data;
        // get rid of new_data
        new_data = nullptr;
        delete[] new_data;
        free_slots = _size;
    }
};

/**
  * Bucket sort; takes an array to sort 'A', its size 'n' and the number of buckets 'k'. Builds an
  * array of List objects as buckets
  */
void bucket_sort(double* A, const std::size_t n, const std::size_t k) {
    // allocate an array of empty lists, as many as the number of buckets
    List<double>* B{new List<double>[k]{}};
    // append the values of A to the corresponding bucket
    for (std::size_t i=0; i < n; ++i) {
        B[int(floor(A[i] * k))].append(A[i]);
    }
    std::size_t i{0};
    // iterate over the buckets
    for (std::size_t j=0; j < k; ++j) {
        List<double>& bucket = B[j];
        // sort the bucket
        bucket.sort();
        // reverse the bucket into A
        for (std::size_t l=0; l < B[j].size(); ++l) {
            A[i] = B[j][l];
            ++i;
        }
    }
    // deallocate
    delete[] B;
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
/*
    // the following block of code tests the exeuction time of Counting Sort

    std::cout << "TESTING THE IMPLEMENTATION OF COUNTING SORT" << std::endl;
    for (std::size_t dim : {100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000}) {
        int test[dim];
        int b[dim];
        // initialize array of random values
        for (std::size_t i=0; i < dim; ++i) {
            test[i] = rand()%(2*MAX_VALUE)-MAX_VALUE;
        }
        // measure time
        auto start = std::chrono::high_resolution_clock::now();
        counting_sort(test, b, -MAX_VALUE, MAX_VALUE, dim);
        auto end = std::chrono::high_resolution_clock::now();
        // print result
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    // the following block of code tests the execution time of Radix Sort

    std::cout << "TESTING THE IMPLEMENTATION OF RADIX SORT" << std::endl;
    for (std::size_t dim : {100, 250, 500, 750, 1000, 1250, 2500, 3000, 4000, 5000}) {//, 100000, 500000, 1000000}) {
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

    // the following block of code tests the execution time of Bucket Sort

    std::cout << "TESTING THE IMPLEMENTATION OF BUCKET SORT" << std::endl;
    for (std::size_t dim : {10, 50, 100, 500, 1000, 5000, 10000, 50000}) {
        double test[dim];
        // initialize array of random values
        for (std::size_t i=0; i < dim; ++i) {
            test[i] = rand() / (RAND_MAX + 1.);  // sample from [0, 1]
        }
        // measure time
        auto start = std::chrono::high_resolution_clock::now();
        bucket_sort(test, dim, dim / 2);
        auto end = std::chrono::high_resolution_clock::now();
        // print result
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    // the following block of code tests the execution time of the Select algorithm

    std::cout << "TESTING THE IMPLEMENTATION OF SELECT" << std::endl;
    for (std::size_t dim : {10, 50, 100, 500, 1000, 2500, 5000, 10000, 100000}) {
        double test[dim];
        // initialize array of random values
        for (std::size_t i=0; i < dim; ++i) {
            test[i] = rand()%(2*MAX_VALUE)-MAX_VALUE;
        }
        // randomly pick position to investigate
        int i = rand() % dim;
        // measure time
        auto start = std::chrono::high_resolution_clock::now();
        select(test, i, 0, dim - 1);
        auto end = std::chrono::high_resolution_clock::now();
        // print result
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }*/

    return 0;
}
