/**
  * dynamic array implementation. It stores an array of data and provides a reallocation mechanism for better
  * memory management, so that it can gradually grow in size. It is intended to be used inside 'bucket_sort' for
  * the buckets, as well as the list of buckets, and in Dijkstra's algorithm as an adjacency list implementation.
  */


template<class T>
class List {
    std::size_t free_slots;  // how many slots are still available
    std::size_t _size;  // index of the next element that will be inserted
    T* data;
    /**
      * Helper function to dynamically reallocate the array
      */
    void check_and_alloc() {
        if (free_slots != 0) return;  // if there is room left, do nothing
        T* new_data{new T[_size + 1]}; // since the distribution over the buckets is uniform, we can reallocate by one without significant loss of efficiency
        // copy all the elements into new array
        for (std::size_t i=0; i < _size; ++i) {
            new_data[i] = data[i];
        }
        data = std::move(new_data);  // move semantics are fast and memory efficient
        free_slots = _size;
    }

  public:
    /**
      * Default constructor
      */
    List() : free_slots{0}, _size{0}, data{new T[0]} {}
    /**
      * Constructs from an array of elements of size n
      */
    List(T* array, const std::size_t n) : free_slots{0}, _size{n}, data{new T[n]} {
        for (std::size_t i=0; i < n; ++i) {
            data[i] = array[i];
        }
    }
    /**
      * Returns the size
      */
    std::size_t size() const noexcept {return _size;}
    /**
      * Appends a new value at the end of the list, reallocates if necessary (yes in
      * bucket sort, not in Dijkstra's algorithm
      */
    void append(const T& value) {
        check_and_alloc();
        data[_size] = value;
        ++_size;
        --free_slots;
    }
    /**
      * Sort the bucket using quicksort
      */
    void sort() noexcept {
        quicksort(data, 0, _size);
    }
    /**
      * Overload of operator[], list[i] returns data[i]
      */
    T& operator[](const std::size_t i) const noexcept {return data[i];}
};
