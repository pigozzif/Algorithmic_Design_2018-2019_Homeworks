#include <iostream>
#include <chrono>
#include <cmath>
#include <forward_list>
#include "heap.h"

#define MAX_VALUE 25

/** The main function includes time tests on the free sorting routines here implemented.
 * It turns out that, on average, insertion sort is considerably slower, and its execution
 * can grow quadratically with respect to the problem size. On the other side, quicksort tended
 * to perform very well on small problem instances, but was beaten by heapsort on large arrays.
 * In fatc, we now that the latter grows at most as nlogn, whereas quicksort does so only on average.
 */


/**
   Swap helper function

   Swaps A[i] and A[j]

   Uses move semantics for efficiency
*/

template<class T>
void swap(T* A, const int i, const int j) {
    T temp{A[i]};
    A[i] = A[j];
    A[j] = std::move(temp);
}

/**
   Dichotomic search

  Takes the length of array A and a candidate a

  Returns the index position of a in A, -1 if not found
*/

template<class T>
int di_find(T* A, const T a, int r) {
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

/**
   Insertion sort

   Takes an array to sort and its length

   Applies the aforementioned algorithm in-place
*/

template<class T>
void insertion_sort(T* A, const int n) {
    for (int i=1; i < n; ++i) {
        int j{i};
        while (j > 0 && A[j] < A[j - 1]) {
            swap(A, j-1, j);
            --j;
        }
    }
}

/**
   Partition routine, used by Quicksort and Select

   Takes an array to partition, lower and upper bounds l and r, and pivot position

   Returns the index of the final position of the pivot
*/

template<class T>
int partition(T* A, int l, int r, int p) {
    swap(A, l, p);
    p = l;
    ++l;
    while (l <= r) {
        if (A[l] > A[p]) {
            swap(A, l, r);
            --r;
        }
        else {
            ++l;
        }
    }
    swap(A, p, r);
    return r;
}

/**
   Quicksort

   Takes an array to sort, lower and upper bounds' indexes, and a pointer to a function
   used to choose the pivot

   Sorts in-place and avoids the tail recursion
*/

template<class T>
void quicksort(T* A, int l, int r, int (*func)(const int, const int)=[](const int l, const int r) {(void)r; return l;}) {
    while (l < r) {
        int pivot = partition(A, l, r-1, func(l, r));
        quicksort(A, l, pivot, func);
        l = pivot + 1;
    }
}

/**
   Heapsort

   Takes an array to sort and its length

   Sorts in-place by using move semantics and building a max-heap, defined in the heap.h header
*/

template<class T>
void heapsort(T* A, const std::size_t n) {
    BinaryHeap<T,std::greater<T>> h{A, n, true};   //std::greater allows us to represent a maxheap
    for (std::size_t i=n-1; i >= 1; --i) {
        h.swap(i, 0);
        --h.size;
        ++h.free_slots;
        h.heapify(0);
    }
    A = std::move(h.data);
}

/**
   Counting sort

   Takes an array to sort, the destination array, lower and upper bounds for the domain, and the length of the array

   Does not sort in-place
*/

template<class T>
void counting_sort(T* A, T* B, const std::size_t k1, const std::size_t k2, const std::size_t n) {
    std::size_t k{k2 - k1 + 1};
    int* C{new int[k]};
    for (std::size_t j=0; j < k; ++j) {
        C[j] = 0;
    }
    for (std::size_t i=0; i < n; ++i) {
        ++C[A[i] - k1];
    }
    for (std::size_t j=1; j < k; ++j) {
        C[j] += C[j - 1];
    }
    for (std::size_t i=n; i >= 1; --i) {
        T v{A[i-1]};    // we use i-1 because otherwise the condition is always true for std::size_t
        B[C[v - k1] - 1] = v;
        --C[v - k1];
    }
    delete[] C;
}

/**
   Radix sort

   Takes an array to sort, max number of digits, and the length of the array

   Does not sort in-place, internally uses counting sort modified to work on one digit per time
*/

template<class T>
void radix_sort(T* A, const int d, const std::size_t n) {  // d is the max number of digits
    // C is the counter array, B will store intermediate outputs
    int* C{new int[10]};
    T* B{new T[n]};
    // for each digit j from the least significant to the most significant
    for (int j=1; j < pow(10, d); j*=10) {
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
            T v{A[i-1]};
            B[C[(A[i-1] / j) % 10] - 1] = v;
            --C[(A[i-1] / j) % 10];
        }
        // save intermediate result into A
        for (std::size_t i=0; i < n; ++i) {
            A[i] = B[i];
        }
    }
    delete[] B;
    delete[] C;
}

/**
   List class, plays the role of a bucket to be used in bucket_sort. It works as an

   array that is dynamically reallocated as it grows in size. Notice it is not a linked list
*/

template<class T>
class List {
    std::size_t free_slots;
    std::size_t _size;
    T* data;
  public:
    List() : free_slots{0}, _size{0}, data{new T[0]} {}
    // returns _size
    std::size_t size() const noexcept {
        return _size;
    }
    // inserts a new value
    void append(const T& value) {
        ++_size;   // otherwise check_and_alloc() would allocate an array of size 0
        check_and_alloc();
        data[_size - 1] = value;
        --free_slots;
    }
    // sorts the bucket. Uses quicksort
    void sort() noexcept {
        quicksort(data,0,_size);
    }
    // overload of operator[]
    T& operator[](const std::size_t i) const noexcept {return data[i];}
  private:
    // helper function to dynamically reallocate the array
    void check_and_alloc() {
        if (free_slots != 0) return;
        T* new_data{new T[_size * 2]};
        for (std::size_t i=0; i < _size; ++i) new_data[i] = data[i];
        data = std::move(new_data);
        free_slots = _size;
    }
};

/**
   Bucket sort

   Takes an array to sort and its size

   Builds an array of List objects as buckets
*/

template<class T>
void bucket_sort(T* A, const std::size_t n) {
    List<T>* B{new List<T>[n]{}};
    for (std::size_t i=0; i < n; ++i) {
        B[int(floor(A[i] * n))].append(A[i]);
    }
    std::size_t i{0};
    for (std::size_t j=0; j < n; ++j) {
        auto bucket = B[j];
        bucket.sort();
        for (std::size_t k=0; k < bucket.size(); ++k) {
            A[i] = bucket[k];
             ++i;
        }
    }
    delete[] B;
}

/**
   Main function
*/

int main() {
    //int test[10] = {13, 5, 7, 2, 1, 4, 1, 11, 6, 1};
    //int test[24] = {13, 5, 7, 2, -7, 4, 1, 11, 6, 0, 10, 15, -1, 8, -2, 3, 14, 12, -9, -5, 9, -6, 16, -4};
    float test[10] = {0.23, 0.60, 0.20, 0.73, 0.97, 0.34, 0.01, 0.50, 0.78, 0.44};
    bucket_sort(test, 10);
    //int test2[10];// = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    //int (*choose_pivot) (const int, const int);   // to be passed to quicksort in order to select the pivot
    //choose_pivot = [] (const int l, const int r) -> int {(void)r; return l;};
    //quicksort(test, 0, 10, choose_pivot);
//counting_sort(test, test2, 2, 14, 10);
    //int ans = select(test, 0, 0, 23);
    //std::cout << ans;
    //int test2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    //void (*sort) (int*, 10);
    //sort = &insertion_sort;
    //bucket_sort<int>(test, 10, 10);
    //for (std::size_t i=0; i < 4; ++i) std::cout << test[i] << std::endl;
   /*
    std::cout << "TESTING THE IMPLEMENTATION OF INSERTION_SORT IN THE WORST CASE:" << std::endl;
    for (std::size_t dim : {100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000}) {
        int test[dim];
        // initialize array of reverse-sorted values, which is the worst-case for insertion sort
        for (int i=dim-1; i >= 0; --i) {
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

    int (*choose_pivot) (const int, const int);   // to be passed to quicksort in order to select the pivot
    choose_pivot = [] (const int l, const int r) -> int {(void)r; return l;};  // let it point to a lambda function that always returns the first element, see the worst case
    std::cout << "TESTING THE IMPLEMENTATION OF QUICKSORT IN THE WORST CASE:" << std::endl;
    for (std::size_t dim : {100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000}) {
        int test[dim];
        // initialize array of random values
        for (std::size_t i=0; i < dim; ++i) {
            test[i] = rand()%(2*MAX_VALUE)-MAX_VALUE;
        }
        // measure time
        auto start = std::chrono::high_resolution_clock::now();
        quicksort<int>(test, 0, dim, choose_pivot);
        auto end = std::chrono::high_resolution_clock::now();
        // print result
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

    choose_pivot = [] (const int l, const int r) -> int {return (l + r) / 2;};  // let it point to a lambda function that always returns the median element, see constant partition and best case
    std::cout << "TESTING THE IMPLEMENTATION OF QUICKSORT IN THE BEST CASE:" << std::endl;
    for (std::size_t dim : {100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000}) {
        int test[dim];
        // initialize array of random values
        for (std::size_t i=0; i < dim; ++i) {
            test[i] = rand()%(2*MAX_VALUE)-MAX_VALUE;
        }
        // measure time
        auto start = std::chrono::high_resolution_clock::now();
        quicksort<int>(test, 0, dim, choose_pivot);
        auto end = std::chrono::high_resolution_clock::now();
        // print result
        std::cout << "Size: " << dim << std::endl;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    }

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
    }*/

    return 0;
}
