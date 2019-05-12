#include <iostream>
#include <queue.h>
#include <heap.h>

/**
  * Perform the relaxation step of Dijkstra's algorithm. Check if the candidate distance of v is greater than
  * its parent's distance plus the weight of the edge; if yes, update v's distance and set u to be the predecessor
  * in the shortest-paths tree.
  */
void relax(Queue& q, Vertex& u, Vertex& v, const int w) {
    if (u.d + w < v.d) {
        q[v.index] = u.d + w;  // update the queue
        v.d = u.d + w;
        v.pred = u.index;
    }
}

void relax(BinaryHeap<Vertex, CompareVertex>& q, Vertex& u, Vertex& v, const int w) {
    if (u.d + w < v.d) {
        q.decrease(v.index, u.d + w);  // update the heap
        v.d = u.d + w;
        v.pred = u.index;
    }
}

/**
  * Run Dijkstra's SSSP algorithm on a graph (given |V| and a source vertex), whose representation is given by an adjacency list.
  * In particular, the implementation which has been chosen here is an array of Vertex instances, each of which will have its
  * own adjacency list among the members. This choice is justified by the fact that this algorithm generally works better
  * for sparse graphs, since its asymptotic complexity, despite varying with the queue data structure implementation, depends
  * on the number of edges in every case. In the following, we first present the implementation using arrays as queues (encapsulated
  * in the 'Queue' custom class). Its complexity is Theta(|V|^2 + |E|).
  */
template<class Q>
void dijkstra(Vertex* graph, const std::size_t n, Vertex& s) {
    s.d = 0;  // set source distance to 0
    Q q{graph, n};  // build queue from the vertices
    while (!q.is_empty()) {
        // while the queue is not empty, extract the minimum and mark it as no longer in queue
        Vertex& u = graph[q.extract_min()];
        u.on_queue = false;
        //std::cout << u.index << std::endl;
        // then iterate over the neighbors and perform the relaxation step if necessary
        for (Pair& edge : u.neighbors) {
            if (graph[edge.vertex].on_queue == true) {  // only neighbors which are still in the queue
                relax(q, u, graph[edge.vertex], edge.weight);
            }
        }
    }
}

/**
  * We now present an implementation based on binary heaps, with a custom class coming from the heap.h header. Its
  * complexity is O(log|V| * (|V| + |E|)).
  */
void dijkstra_heap(Vertex* graph, const std::size_t n, Vertex& s) {
    s.d = 0;  // set source distance to 0
    BinaryHeap<Vertex, CompareVertex> q{graph, n};  // build queue from the vertices
    while (!q.is_empty()) {
        // while the queue is not empty, extract the minimum and mark it as no longer in queue
        Vertex& u = graph[q.extract_min()];
        u.on_queue = false;
        // then iterate over the neighbors and perform the relaxation step if necessary
        for (Pair& edge : u.neighbors) {
            if (graph[edge.vertex].on_queue == true) {  // only neighbors which are still in the queue
                relax(q, u, graph[edge.vertex], edge.weight);
            }
        }
    }
}


int main() {
    Vertex* graph = new Vertex[6];
    for (int i=0; i < 6; ++i) {
        graph[i] = Vertex{i};
    }
    graph[0].add_neighbor(Pair(1, 1));
    graph[0].add_neighbor(Pair(2, 5));
    graph[1].add_neighbor(Pair(5, 15));
    graph[2].add_neighbor(Pair(3, 2));
    graph[3].add_neighbor(Pair(4, 1));
    graph[4].add_neighbor(Pair(5, 3));
    dijkstra<Queue>(graph, 6, graph[0]);
    //dijkstra_heap(graph, 6, graph[0]);
    for (int i=0; i < 6; ++i) {
        std::cout << "node number: " << graph[i].index << " has distance: " << graph[i].d << std::endl;
    }
    return 0;
}
