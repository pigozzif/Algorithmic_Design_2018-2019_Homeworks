/**
  * This header file contains utility functions to be used in the source files of the current folder.
  * Notice the implementation of 'insertion_sort' has been left here, since it is required by more
  * then one other file. Its tests are in the 'comparison_sort.cc' file
  */

#ifndef __SORT_UTILS__
#define __SORT_UTILS__

/**
  * Swap helper function. Swaps A[i] and A[j]
  */
template<class T>
void swap(T* A, const int i, const int j) {
    T temp{A[i]};
    A[i] = A[j];
    A[j] = temp;  // this could have been made more efficient using move semantics; we stick to
    // the assignment and avoid the STL as much as possible
}

/**
  * Insertion sort; takes an array to sort 'A' and its length 'n'.
  * Applies the aforementioned algorithm, which works in-place
  */
template<class T>
void insertion_sort(T* A, const int n) {
    // for every element in the array, the first is trivially sorted with respect to itself
    for (int i=1; i < n; ++i) {
        // start from the element next to the last element sorted
        int j{i};
        // move to the left and and swap each element with the previous one until find the
        // partially sorted position of the current element
        while (j > 0 && A[j] < A[j - 1]) {
            swap(A, j - 1, j);
            --j;
        }
    }
}

/**
  * Partition routine, used by Quicksort and Select. Takes an array to partition 'A',
  * lower and upper bounds 'l' and 'r', and pivot position 'p'. Returns the index of
  * the final position of the pivot. Moreover, it makes sure all the duplicates of the
  * pivot are to its left
  */
template<class T>
int partition(T* A, int l, int r, int p) {
    // switch the pivot and the first element
    swap(A, l, p);
    p = l;
    ++l;
    while (l <= r) {
        // if A[i] is greater than the pivot place it in the "greater" part
        if (A[l] > A[p]) {
            swap(A, l, r);
            --r;
        }
        // else A[i] is already in "smaller" part
        else {
            ++l;
        }
    }
    swap(A, p, r);  // place the pivot between the "smaller" and "greater" parts
    return r;
}

#endif // __SORT_UTILS__
