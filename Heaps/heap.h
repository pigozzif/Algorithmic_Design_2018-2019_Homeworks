#include <iostream>
#include <stdexcept>

// some useful macros to define heap invariants
#define LEFT(x) 2*x + 1
#define RIGHT(x) 2*x + 2
#define PARENT(x) (x - 1) / 2
#define GET_ROOT() 0
#define IS_ROOT(x) x == 0

template<class T, class Comp>
struct BinaryHeap {
    // data
    std::size_t size;
    std::size_t free_slots;
    T* data;    // array of elements
    Comp compare;
    // constructor (BUILD_HEAP procedure). Notice the call to std::move allows us to implement heapsort in place
    BinaryHeap(T* array, const std::size_t n, const bool inplace=false) : size{n}, free_slots{0}, data{new T[size]}, compare{} {
        if (inplace) {
            data = std::move(array);
        }
        else {
            for (std::size_t i=0; i < size; ++i) {
                data[i] = array[i];
            }
        }
        for (int i=PARENT(size-1); i >= 0; --i) {  //int because the condition is always true for std::size_t
            heapify(i);
        }
    }
    // overload of operator[]
    T& operator[](const std::size_t i) noexcept {return data[i];}
    // HEAP_MINIMUM procedure to remove the minimum, or whatever optimum
    T get_opt() noexcept;
    // HEAP_DECREASE_KEY procedure. Throws std::invalid_argument if value is greater than or equal to data[i]
    void decrease(std::size_t i, const T& value);
    // HEAP_INSERT procedure
    void insert(const T& value);
    // HEAPIFY routine, iterative version
    void heapify(std::size_t i) noexcept;
    // swap helper function
    void swap(const std::size_t i, const std::size_t m) noexcept {
        T temp{data[m]};
        data[m] = data[i];
        data[i] = std::move(temp);
    }
    // utility function to check whether an index corresponds to a valid node
    bool is_valid_node(const std::size_t i) const noexcept {return size-1 >= i;}
    // utility function to manage reallocation. For simplicity, it doubles the array each time
    void check_and_alloc() {
        if (free_slots != 0) return;
        T* new_data{new T[size * 2]};
        for (std::size_t i=0; i < size; ++i) new_data[i] = data[i];
        data = std::move(new_data);
        free_slots = size;
    }
    // bubble-up helper function
    void bubble_up(std::size_t i) noexcept;

};

template<class T, class Comp>
void BinaryHeap<T,Comp>::heapify(std::size_t i) noexcept {
    std::size_t m{i};
    // iterate as long as the current node doesn't satisfy the heap propriety, else break
    while (true) {
        for (auto j : {LEFT(i), RIGHT(i)}) {
            if (is_valid_node(j) && compare(data[j], data[m])) m = j;
        }
        if (i != m) {
            swap(i, m);
            i = m;
        }
        else break;
    }
}

template<class T, class Comp>
void BinaryHeap<T,Comp>::bubble_up(std::size_t i) noexcept {
    while (!(IS_ROOT(i)) && compare(data[i], data[PARENT(i)])) {
        swap(i, PARENT(i));
        i = PARENT(i);
    }
}

template<class T, class Comp>
T BinaryHeap<T,Comp>::get_opt() noexcept {
    T ans{data[0]};
    data[0] = data[size - 1];
    --size;
    ++free_slots;
    heapify(GET_ROOT());
    return ans;
}

template<class T, class Comp>
void BinaryHeap<T,Comp>::decrease(std::size_t i, const T& value) {
      if (compare(data[i], value)) throw std::invalid_argument("value is not smaller than H[i]");
      data[i] = value;
      bubble_up(i);
}

template<class T, class Comp>
void BinaryHeap<T,Comp>::insert(const T& value) {
    check_and_alloc();
    data[size] = value;
    bubble_up(size);
    ++size;
    --free_slots;
}
