#include <iostream>
#include <chrono>
#include <queue.h>
#include <heap.h>


/**
  * Run Dijkstra's SSSP algorithm on a graph, given the an array of Vertex instances, their number and a reference to the source vertex.
  * The representation of the graph is given by an adjacency matrix, since the adjacency list representation turned out to be somewhat complex
  * to program without the STL. The complexity is Theta(|V|^2 + |E|) with an array-based implementation of the queue data structure (the class
  * 'Queue' coming from the queue.h header), while O(log|V| * (|V| + |E|)) with a BinaryHeap data sructure, which had been implemented in the
  * heap.h header for a previous assignment. Notice the template defines the queue data structure to use.
  */
template<class Q>
void dijkstra(int** graph, Vertex V[], const std::size_t n, Vertex& s) {
    s.d = 0;  // set source distance to 0
    Q q{V, n};  // build queue from the vertices
    while (!q.is_empty()) {
        // while the queue is not empty, extract the minimum and mark it as no longer in queue
        Vertex& u = V[q.extract_min()];
        u.on_queue = false;
        // then iterate over the neighbors (a row in the adjacency matrix) and perform the relaxation step, if necessary
        for (std::size_t i=0; i < n; ++i) {
            int w = graph[u.index][i];
            Vertex& v = V[i];
            // -1 stands for no edge. Notice this is not an issue, since Dijkstra's algorithm is not intended for graphs with negative weights
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
    Vertex* vertices = new Vertex[6];
    int** graph = new int*[6];
    for (int i = 0; i < 6; ++i) {
        graph[i] = new int[6]{-1};
        for (int j=0; j < 6; ++j) graph[i][j] = -1;  // fill the matrix with -1, our default for no edge
        vertices[i] = Vertex{i};
    }
    // define first custom graph. This graph is the same illustrated in lecture
    graph[0][1] = 1;
    graph[0][2] = 5;
    graph[1][5] = 15;
    graph[2][3] = 2;
    graph[3][4] = 1;
    graph[4][5] = 3;
    // test first with BinaryHeap
    std::cout << "Tests with graph number one" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    dijkstra<BinaryHeap<Vertex, CompareVertex>>(graph, vertices, 6, vertices[0]);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "BinaryHeap implementation: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << std::endl;
    // reset vertices' members
    for (int i=0; i < 6; ++i) {
        vertices[i].pred = -1;
        vertices[i].d = INT_MAX;
    }
    // test with Queue
    start = std::chrono::high_resolution_clock::now();
    dijkstra<Queue>(graph, vertices, 6, vertices[0]);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Array implementation: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << std::endl;
    //for (int i=0; i < 6; ++i) {
    //    std::cout << "node number: " << vertices[i].index << " has distance: " << vertices[i].d << std::endl;
    //}
    // deallocate
    for (int i = 0; i < 6; ++i) {
        delete[] graph[i];
    }
    delete[] graph;
    delete[] vertices;
    return 0;
}
