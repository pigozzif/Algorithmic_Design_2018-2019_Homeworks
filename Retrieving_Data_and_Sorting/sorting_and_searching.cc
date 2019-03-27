#include <iostream>
#include <chrono>
#include <cmath>
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
void swap(T* A, const int i, const int j) {
    T temp{A[i]};
    A[i] = A[j];
    A[j] = std::move(temp);
}

// dichotomic search

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

// insertion sort

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

// partition routine

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

// quicksort, avoiding the tail recursion

template<class T>
void quicksort(T* A, int l, int r, int (*func)(const int, const int)) {
    while (l < r) {
        int pivot = partition(A, l, r-1, func(l, r));
        quicksort(A, l, pivot, func);
        l = pivot + 1;
    }
}

// heapsort

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

// counting sort. Generalizes to any domain bounded by k1 as the minimum and k2 as the maximum

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

// radix sort

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
        // save intermdiate result into A
        for (std::size_t i=0; i < n; ++i) {
            A[i] = B[i];
        }
    }
    delete[] B;
    delete[] C;
}

// bucket sort

template<class T>
class Bucket {
    struct BucketNode {
        T data;
        BucketNode* next;
        BucketNode(T& val) : data{val}, next{nullptr} {}
    };

    BucketNode* current;
    BucketNode* head;

  public:
    std::size_t size;
    Bucket() : size{}, current{nullptr}, head{nullptr} {}
    void append(const T& value) {
        BucketNode node{value};
        head->next = node;
        head = node;
        ++size;
    }
    T get() {
        T ans{current->data};
        current = current->next;
        return ans;
    }
};

/*
template<class T, class Sort, class Args>
void bucket_sort(T* A, const std::size_t n, Args ... args) {
    //Sort sorter{};
    Bucket<T>* B{new Bucket<T>[n]{}};
    for (std::size_t i=0; i < n; ++i) {
        B[floor(A[i] / n) + 1].append(A[i]);
    }
    std::size_t i{0};
    for (std::size_t j=0; j < n; ++j) {
        Bucket<T> curr = B[j];
        for (std::size_t k=0; k < curr.size; ++k) {
            A[i] = curr.get();
            ++i;
        }
        sorter(curr, args..., );
    }
    delete[] B;
}
*/
template<class T>
int select(T*A, int i, int l, int r, int threshold=0);

template<class T>
int select_pivot(T* A, int l, int r) {
    for (int i=0; i <= (r - l)/5; ++i) {
        int a{5*i + l + 4};
        int k{5*i + l};
        insertion_sort(A + k, (a > r) ? r - k + 1: a - k + 1);
    }
    for (int i=0; i <= (r - l)/5; ++i) {
        int a{5*i + l + 2};
        swap(A, l+i, (a > r) ? a : r);
    }
    for (int i=l; i < l + (r - l)/5; ++i) std::cout << A[i] << std::endl;
    auto ans = select(A, ((r - l)/5 + 1)/2, l, l + (r - l)/5);
    //std::cout << ans;
    //abort();
    return ans;
}

template<class T>
int select(T* A, int i, int l, int r, int threshold) {
    if ((r - l) <= threshold) {
        insertion_sort(A, r - l);
        //for (int i=l; i < r; ++i) std::cout << A[i] << std::endl;
        return i;
    }
    //std::cout << r << " " << l << std::endl;
    int j{select_pivot(A, l, r)};
    int k{partition(A, l, r, j)};
    if (i == k) {
        return k;
    }
    else if (i < k) {
        return select(A, i, l, k - 1);
    }
    return select(A, i, k + 1, r);
}

int main() {
    //int test[10] = {13, 5, 7, 2, 1, 4, 1, 11, 6, 1};
    //int test[24] = {13, 5, 7, 2, -7, 4, 1, 11, 6, 0, 10, 15, -1, 8, -2, 3, 14, 12, -9, -5, 9, -6, 16, -4};
    int test[4] = {3475, 2475, 3471, 1733};
    radix_sort(test, 4, 4);
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
    for (std::size_t i=0; i < 4; ++i) std::cout << test[i] << std::endl;
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
