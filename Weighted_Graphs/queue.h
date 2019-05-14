#include <iostream>
#include <math.h> // for INT_MAX


/**
  * Dynamic array implementation. It stores an array of data and provides a reallocation mechanism for better
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
    List() = default;
    List(const std::size_t n) : free_slots{n}, _size{0}, data{new T[free_slots]} {}
    /**
      * Begin and end functions, necessary to support the range for loop over the List. They return an iterator
      * to the first element of 'data' and to the first past the last one, respectively.
      */
    T* begin() const {return &data[0];}
    T* end() const {return &data[_size];}
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
    /**
      * Destructor
      */
    ~List() {delete[] data;}
};

//struct Pair;  // preemptive declaration to be used in 'Vertex'

/**
  * Implementation of the vertex data structure, to be used as part of a graph. The members have been modeled in function
  * of what needs to be done in Dijkstra's algorithm. Notice the List member 'neighbors' allows us to implement an
  * adjacency list representation of a graph. Without loss of generality, we can use integers from 0 to label vertices,
  * since the user can always define an index (like an hashtable, or a tree) to map from the original labels to the integers.
  * Notice the integer -1 has been used as a proxy for NULL in the 'pred' member.
  */

struct Vertex {
    int index;  // index of the vertex
    int d;  // distance
    int pred;  // predecessor in the shortest-paths tree
    //int* neighbors;  // adjacency list of the neighbors
    bool on_queue;  // if the vertex is still in the queue
    /**
      * Default constructor
      */
    Vertex() = default;
    /**
      * Constructs a node from a given index. Notice all the nodes are assumed to be on the queue at the beginning.
      * Even if this might sound like a weird assumption, remember that this implementation is Dijkstra-focused.
      */
    Vertex(const int idx) : index{idx}, d{INT_MAX}, pred{-1}, on_queue{true} {}
    /**
      * Default destructor
      */
    ~Vertex() = default;
};

/**
  * Struct to encapsulate the index of a vertex together with a weight. This is intended to represent an edge
  * in the adjacency list of a Vertex instance. Without loss of generality, we can use integers for the vertices.
  * Notice this implementation is quite close to the std::pair of the STL, defined in the <utility> header.
  */

//struct Pair {
//    int vertex;  // the vertex of the (v, w) pair
//    int weight;  // the weight of the (v, w) pair
    /**
      * Default constructor
      */
//    Pair() = default;
    /**
      * Constructs a Pair from v, w
      */
//    Pair(const int v, const int w) : vertex{v}, weight{w} {}
    /**
      * Default destructor
      */
//    ~Pair() = default;
//};

/**
  * Function object for Vertex comparisons. Notice the overloads of the operator() allow us to compare two
  * instances of Vertex, or a Vertex with an integer. All comparisons are based on the member 'd', the distance.
  * To be passed as a template parameter to the BinaryHeap in Dijkstra's algorithm.
  */
struct CompareVertex {
    CompareVertex() = default;
    bool operator()(const Vertex& a, const Vertex&b) const noexcept {
        return a.d < b.d;
    }
    bool operator()(const Vertex& vertex, const int value) const noexcept {
        return vertex.d < value;
    }
    ~CompareVertex() = default;
};

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
    std::size_t num;  // number of elements still in the queue
    int* data;  // array of data to store

  public:
    /**
      * Constructor, builds a queue by copying the distance associate to each Vertex into 'data', whose size is defined by n.
      * 'array' is an adjency list representation of a graph. Equivalent to BUILD_QUEUE. Since we have to copy once each
      * element, the complexity is Theta(n)
      */
    Queue(Vertex graph[], const std::size_t n) : size{n}, free_slots{0}, num{n}, data{new int[size]} {
        // copy the array elements one by one
        for (std::size_t i=0; i < n; ++i) {
            data[i] = graph[i].d;
        }
    }
    /**
      * Tests whether the queue does not contain any element
      */
    bool is_empty() const noexcept {
        return num == 0;
    }
    /**
      * Finds the minimum of the array and returns it, effectively washing it
      * away from the data. Since we have to iterate over all the elements twice,
      * the complexity is Theta(2n) = Theta(n)
      */
    std::size_t extract_min() noexcept {
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
        data[index] = INT_MAX;  // set the "extracted" element's distance to the maximum, to effectively drop from further consideration
        // update members and return the minimum
        --num;
        ++free_slots;
        return index;
    }
    /**
      * Overload of operator[], queue[i] returns the element in position i of 'data'.
      * Notice that returning by reference allows us to update the distance as if it were
      * UPDATE_DISTANCE. Complexity is Theta(1)
      */
    void decrease(const std::size_t i, const int value) {
        data[i] = value;
    }
    /**
      * Destructor
      */
    ~Queue() {delete[] data;}
};
