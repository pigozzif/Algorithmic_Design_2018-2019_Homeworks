#ifndef __HEAP__
#define __HEAP__

#include <iostream>
#include <math.h>

// some useful macros to define heap invariants
#define LEFT(x) 2*x + 1
#define RIGHT(x) 2*x + 2
#define PARENT(x) (x - 1) / 2
#define GET_ROOT() 0
#define IS_ROOT(x) x == 0

/**
  * The Heap class is templated on the type of the data to store and the type of comparison
  * to define the heap propriety. It implements all the heap algorithms touched in lecture.
  * In the 'heap.cc' script is possible to run a test to benchmark the execution time of HEAPIFY,
  * and, as expected, it turns out to be O(logn), where n is the number of nodes of the heap.
  */
template<class T, class Comp>
struct BinaryHeap {
    // data
    std::size_t size;  // size of the heap
    T* data;  // array of elements
    Comp compare;  // comparison operator
    // constructor (BUILD_HEAP procedure). Notice the call to std::move allows us to implement heapsort in place.
    // Takes the 'array' to take elements from, its size 'n', and a boolean flag 'inplace'
    BinaryHeap(T* array, const std::size_t n, const bool inplace=false) : size{n}, data{}, compare{} {
        if (inplace) {
            // repositioning the pointer allows us to work with one copy of the same array.
            // Notice that, as a result, any change to the heap array will be reflected on
            // the outer array as well
            data = array;
        }
        // if build is not in-place, allocate an array on the heap and copy elements one by one from 'array'
        else {
            data = new T[size];
            for (std::size_t i=0; i < size; ++i) {
                data[i] = array[i];
            }
        }
        // call HEAPIFY bottom-up
        for (int i=PARENT(size-1); i >= 0; --i) {  // int because the condition is always true for std::size_t
            heapify(i);
        }
    }
    // overload of operator[], returns by reference. Used in Dijkstra's algorithm implementation
    T& operator[](const std::size_t i) noexcept {
        return data[i];
    }
    bool is_empty() const noexcept {
        return size == 0;
    }
    // HEAP_MINIMUM procedure to remove the minimum, or whatever optimum.
    // Instrumented for Dijkstra's algorithm
    int extract_min() noexcept {
        // extract the root and replace it with the rightmost leaf
        int ans = data[0].index;
        data[0] = data[size - 1];
        // update size and free space
        --size;
        // call heapify on the root
        heapify(GET_ROOT());
        return ans;
    }
    // HEAP_DECREASE_KEY procedure; decrease the element H[i] to 'value'
    void decrease(std::size_t i, const T& value) {
        // if the value does not imply a decrease, abort the program
        if (compare(data[i], value)) {
            std::cout << "value is not smaller than H[i]" << std::endl;
            abort();
        }
        data[i] = value;
        // push the problem one level up to the root
        bubble_up(i);
    }
    // Same as above, but using an integer as new value. Necessary for binary heap-based implementation of
    // Dijkstra's algorithm, since the 'DECREASE' operation applies specifically to the .d member of the
    // Vertex class
    void decrease(std::size_t i, const int value) {
        // if the value does not imply a decrease, abort the program
        if (compare(data[i], value)) {
            std::cout << "value is not smaller than H[i]" << std::endl;
            abort();
        }
        data[i].d = value;
        bubble_up(i);
    }
    // HEAPIFY routine, iterative version
    void heapify(std::size_t i) noexcept {
          std::size_t m{i};
          // iterate as long as the current node doesn't satisfy the heap propriety, else break
          while (true) {
                // for the left and right children, if it is a valid node and the key is smaller or equal
                // (borrowing notation from a min-heap) than the current node, let m be equal to it
                if (is_valid_node(LEFT(i)) && compare(data[LEFT(i)], data[m])) {
                    m = LEFT(i);
                }
                if (is_valid_node(RIGHT(i)) && compare(data[RIGHT(i)], data[m])) {
                    m = RIGHT(i);
                }
                // if i is different from m, then one of the two children breaks the heap propriety
                // and a swap is necessary to push the problem one level down
                if (i != m) {
                    swap(i, m);
                    i = m;
                }
                // if such a situation did not occur, by induction the heap propriety must hold heap-wise
                // and we can stop
                else break;
           }
    }
    // swap helper function
    void swap(const std::size_t i, const std::size_t m) noexcept {
        T temp{data[m]};
        data[m] = data[i];
        // this could have been made more efficient using move semantics,
        // but we will stick to the assignment and avoid the STL as much as possible
        data[i] = temp;
    }
    // utility function to check whether an index corresponds to a valid node
    bool is_valid_node(const std::size_t i) const noexcept {return size-1 >= i;}
    // bubble-up helper function
    void bubble_up(std::size_t i) noexcept {
        // if the node is not the root and violates the heap propriety with respect to
        // its parent, swap them and move up
        while (!(IS_ROOT(i)) && compare(data[i], data[PARENT(i)])) {
            swap(i, PARENT(i));
            i = PARENT(i);
        }
    }
    // destructor
    ~BinaryHeap() {
    }
};

#endif // __HEAP__
