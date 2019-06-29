#ifndef __GRAPH_UTIL__
#define __GRAPH_UTIL__

/**
  * This header file includes structs and classes used by the Dijkstra's algorithm implementation of dijkstra.cc
  */

#include <iostream>
#include <math.h> // for INT_MAX


/**
  * Implementation of the vertex data structure, to be used as part of a graph. The members have been modeled in function
  * of what needs to be done in Dijkstra's algorithm. Without loss of generality, we can use integers from 0 to label vertices,
  * since the user can always define an index (like an hashtable) to map from the original labels to the integers.
  * Notice the integer -1 has been used as a proxy for NULL in the 'pred' member.
  */

struct Vertex {
    int index;  // index of the vertex
    int d;  // distance
    int pred;  // predecessor in the shortest-paths tree
    bool on_queue;  // if the vertex is still in the queue
    // Default constructor
    Vertex() = default;
    // Constructs a node from a given index 'idx'. Notice all the nodes are assumed to be on the queue at the beginning
    Vertex(const int idx) : index{idx}, d{INT_MAX}, pred{-1}, on_queue{true} {}
    // Default destructor
    ~Vertex() = default;
};

/**
  * Function object for Vertex comparisons. Notice the overloads of the operator() allow us to compare two
  * instances of Vertex, or a Vertex with an integer. All comparisons are based on the member 'd', the distance.
  * To be passed as a template parameter to the BinaryHeap in Dijkstra's algorithm.
  */
struct CompareVertex {
    CompareVertex() = default;
    bool operator()(const Vertex& a, const Vertex& b) const noexcept {
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
  * no insertions are possible after the constructor has been called. Since the queue
  * keeps distances, which are integers, templating is not necessary
  */
class Queue {
    std::size_t size;  // the index for the next element after the last one
    std::size_t free_slots;  // the number of slots available, necessary for reallocation
    std::size_t num;  // number of elements still in the queue
    int* data;  // array of data to store

  public:
    // Constructor, builds a queue by copying the distance associated to each Vertex into 'data', whose size is defined by 'n'.
    // 'array' is an adjency list representation of a graph. Equivalent to BUILD_QUEUE.
    Queue(Vertex graph[], const std::size_t n) : size{n}, free_slots{0}, num{n}, data{new int[size]} {
        // copy the array elements one by one
        for (std::size_t i=0; i < n; ++i) {
            data[i] = graph[i].d;
        }
    }
    // Tests whether the queue does not contain any element
    bool is_empty() const noexcept {
        return num == 0;
    }
    // Finds the minimum of the array and returns it, effectively washing it
    // away from the data
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
    // Update the value of data[i] to 'value', just like
    // UPDATE_DISTANCE
    void decrease(const std::size_t i, const int value) {
        data[i] = value;
    }
    // Destructor
    ~Queue() {
        delete[] data;
    }
};

#endif  // __GRAPH_UTIL__
