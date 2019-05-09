#include <iostream>
#include <math.h> // for INT_MAX

/**
  * Queue data structure implementation using arrays. It keeps an internal array of data,
  * which can be manipulated using the extract_min and operator[] functions. This
  * is intended to store the nodes during the computation of Dijkstra's algorithm, so
  * no insertions are possible after the constructor has been called. Since the queue has
  * to keep distances, which are integers, templating is not necessary
  */


class Queue {
    std::size_t size;  // the index for the next element after the last one
    std::size_t free_slots;  // the number of slots available, necessary for reallocation
    int* data;  // array of data to store

  public:
    /**
      * Constructor. Copies 'array' into 'data', whose size is defined by n. Equivalent
      * to BUILD_QUEUE. Of course, the complexity is Theta(n)
      */
    Queue(int* array, const std::size_t n) : size{n}, free_slots{0}, data{new int[size]} {
        // copy the array elements one by one
        for (std::size_t i=0; i < n; ++i) {
            data[i] = array[i];
        }
    }
    bool is_empty() const noexcept {
        return size == 0;
    }
    /**
      * Finds the minimum of the array and returns it, effectively washing it
      * away from the data. Since we have to iterate over all the elements twice,
      * the complexity is Theta(2n) = Theta(n)
      */
    int extract_min() noexcept {
        // trivial algorithm for finding the minimum of an array. At the end,
        // 'minimum' will store the minimum of the array and 'index' its index
        int minimum{INT_MAX};
        std::size_t index{0};
        for (std::size_t i=0; i < size; ++i) {
            if (minimum > data[i]) {
                minimum = data[i];
                index = i;
            }
        }
        // move on from 'index' and copy all the elements to the previous position
        for (std::size_t i=index; i < size-1; ++i) {
            data[i] = data[i + 1];
        }
        // update members and return the minimum
        --size;
        ++free_slots;
        return minimum;
    }
    /**
      * Overload of operator[], queue[i] returns the element in position i of 'data'.
      * Notice that returning by reference allows us to update the distance as if it were
      * UPDATE_DISTANCE. Complexity is Theta(1)
      */
    int& operator[](const std::size_t i) {
        return data[i];
    }
    /**
      * Default destructor
      */
    ~Queue() = default;
};
