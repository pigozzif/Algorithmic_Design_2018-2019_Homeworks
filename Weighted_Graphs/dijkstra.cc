#include <iostream>
#include <chrono>

#include "graph_utilities.h"
#include "heap.h"

#define N 6  // number of vertices of the graph


/**
  * Run Dijkstra's SSSP algorithm on a graph, given an array of Vertex instances 'V', their number 'n' and a reference to the source vertex 's'.
  * The representation of the graph is given by an adjacency matrix. Notice the template defines the queue data structure to use, which will be
  * (at least in our tests) either an array-based implementation of the queue data structure (the class 'Queue' coming from the graph_utilities.h
  * header file), or a BinaryHeap data structure, implemented in the heap.h header for a previous assignment.
  */
template<class Q>
void dijkstra(int graph[][N], Vertex V[], const std::size_t n, Vertex& s) {
    s.d = 0;  // set source distance to 0
    Q q{V, n};  // build queue from the vertices
    // iterate until while there are still nodes to finalize
    while (!q.is_empty()) {
        // while the queue is not empty, extract the minimum and mark it as no longer in queue
        Vertex& u = V[q.extract_min()];
        u.on_queue = false;
        // then iterate over the neighbors (a row in the adjacency matrix) and perform the relaxation step, if necessary
        for (std::size_t i=0; i < n; ++i) {
            int w = graph[u.index][i];  // the weight of the edge
            Vertex& v = V[i];  // the neighbor
            // -1 stands for no edge. Notice this is not a constraint, since Dijkstra's algorithm is not intended for graphs with negative weights
            if (w != -1 && v.on_queue == true) {  // only neighbors which are still in the queue
                // perform the relaxation step of Dijkstra's algorithm. Check if the candidate distance of v is greater than
                // its parent's distance plus the weight of the edge
                if (u.d + w < v.d) {
                    q.decrease(v.index, u.d + w);  // update the queue
                    v.d = u.d + w;  // update v's distance
                    v.pred = u.index;  // set u to be the predecessor in the shortes-path tree
                }
            }
        }
    }
}


int main() {
    // initialize list of vertices and adjacency matrix, which will be a pointer to pointer
    Vertex* vertices = new Vertex[N];
    for (int i = 0; i < N; ++i) {
        vertices[i] = Vertex{i};
    }
    // define custom graph. This graph is the same illustrated in lecture
    int graph[N][N] = {{-1, 1, 5, -1, -1, -1}, {-1, -1, -1, -1, -1, 15}, {-1, -1, -1, 2, -1, -1}, {-1, -1, -1, -1, 1, -1}, {-1, -1, -1, -1, -1, 3}, {-1, -1, -1, -1, -1, 3}};
    // print the adjacency matrix
    //for (int i=0; i < 6; ++i) {
    //    for (int j=0; j < 6; ++j) std::cout << graph[i][j] << " ";
    //    std::cout << std::endl;
    //}
    // test first with BinaryHeap
    std::cout << "Tests with lecture graph" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    dijkstra<BinaryHeap<Vertex, CompareVertex>>(graph, vertices, N, vertices[0]);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "BinaryHeap implementation: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << std::endl;
    // check result
    for (int i=0; i < 6; ++i) {
        std::cout << "node number: " << vertices[i].index << " has distance: " << vertices[i].d << std::endl;
    }
    // reset vertices' members
    for (int i=0; i < 6; ++i) {
        vertices[i].on_queue = true;
        vertices[i].pred = -1;
        vertices[i].d = INT_MAX;
    }
    // test with Queue
    start = std::chrono::high_resolution_clock::now();
    dijkstra<Queue>(graph, vertices, N, vertices[0]);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Array implementation: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << std::endl;
    // check result
    for (int i=0; i < 6; ++i) {
        std::cout << "node number: " << vertices[i].index << " has distance: " << vertices[i].d << std::endl;
    }
    // deallocate
    delete[] vertices;
    return 0;
}
