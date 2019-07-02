#include <iostream>
#include <chrono>

#include "sort_utils.h"

#define MAX_VALUE 25  // maximum value for test items

/**
  * This is a variant of the 'partition' procedure implemented before. Similarly, it works on
  * the array moving from both ends, but the focus here is to compact all the duplicates of
  * the pivot at the end of the subarray (the pivot itself will then occupy the last position)
  */
template<class T>
int partition_for_select(T* A, int l, int r, int p) {
    // switch the pivot and the first element
    swap(A, l, p);
    // initialize placeholders we will use to move
    int left = l + 1;
    int right = r;
    // until there is a subarray to consider. Notice these two conditions are
    // enough to guarantee the loop terminates, since we assume the array has
    // already undergon 'partition', so there are no values greater in the pivot
    // to consider
    while (left <= right) {
        // if A[left] < pivot, move up
        if (A[left] < A[l]) {
            ++left;
        }
        // else if it A[left] == pivot, swap that with A[right] and move
        // in both directions
        else if (A[left] == A[l]) {
            swap(A, left, right);
            ++left;
            --right;
        }
   }
   // replace the pivot to its correct position
   swap(A, right, l);
   // 'right' is now the first occurence of the pivot in the subarray
   return right;
}

/**
  * Helper routine for the 'select' algorithm. It returns the position in the array
  * 'A', with 'l' and 'r' lower and upper bounds, of the optimal pivot according to
  * the median of medians algorithm
  */
template<class T>
int select_pivot(T* A, int l, int r) {
    // easiest case: the array contains only one chunk; sort it and
    // return the median's index
    if ((r - l + 1) <= 5) {
        insertion_sort(A + l, r - l + 1);
        return (l + r + 1) / 2;
    }
    // split A into ceil(n/5) chunks and sort them one after the other
    // i will then be the index of the first element in the chunk
    for (int i=l; i <= r; i+=5) {
        int a{i + 4};  // index of the last element in the chunk
        // if r - l + 1 is not a multiple of 5, the last chunk will be incomplete
        // and we need to take care of that
        insertion_sort(A + i, (a > r) ? (r - l + 1) % 5 : 5);
    }
    int idx{l};  // counter starting at the beginning of the array
    // move the medians to the beginning of the array
    for (int i=l; i <= r; i+=5) {
        int a{i + 2};  // the median index
        swap(A, l + idx, (a > r) ? r : a);
        ++idx;
    }
    int n = (r - l + 1) / 5;  // total number of chunks
    // call 'select_pivot' to recursively find the median of the medians
    return select_pivot(A, l, l + n);
}

/**
  * Select algorithm implementation. It returns the position of the element that, if
  * the input array 'A' were sorted, would be found at index 'i'. Takes the lower and upper
  * bounds 'l' and 'r' (which, for the very first call, are going to be 0 and the index of the last
  * element), and a threshold size (which has been set to 10) below which the
  * array is simply sorted. Notice this implementation avoids tail recursions and uses iteration for
  * greater efficiency; the recursive calls can be found commented
  */
template<class T>
int select(T* A, int i, int l, int r, int threshold=10) {
    while (true) {
        // if the size is smaller than the threshold, simply
        // sort the array and return lower_bound + i
        if ((r - l + 1) <= threshold) {
            insertion_sort(A + l, r - l + 1);
            return l + i;
        }
        // select the pivot
        int j = select_pivot(A, l, r);
        // use the selected pivot to partition the array. In the following, we will
        // exploit the fact that the pivot will be in its correct position with
        // respect to the sorted array, all the elements before are smaller and
        // all the elements after are greater
        int k = partition(A, l, r, j);
        // if the pivot is smaller than the position we are looking for,
        // continue in the 'greater' portion of the array
        if (i > k) {
            l = k + 1;
            //return select(A, i, k + 1, r);
        }
        // else consider the case of duplicate values
        else {
            // let 'start' be the index of the first occurence of the pivot, where
            // all the duplicates have been compacted before the pivot
            int start = partition_for_select(A, l, k, k);
            // if i is in the range [start, k], then it is equal to the pivot and return it
            if (start <= i <= k) {
                return k;
            }
            // else continue in the 'smaller' portion of the array, leaving aside
            // the duplicates' section
            else if (i < start) {
                r = start - 1;
                //return select(A, i, l, start - 1);
            }
        }
    }
}

/**
  * Main function
  */
int main() {
    std::cout << "TESTING THE IMPLEMENTATION OF SELECT" << std::endl;
    for (std::size_t dim : {10, 50, 100, 500, 1000, 2500, 5000, 10000, 100000, 1000000}) {
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
    }
    return 0;
}
