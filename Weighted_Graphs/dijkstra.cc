#include <iostream>
#include <queue.h>

int main() {
    int test[5] = {1, 2, 3, 4, 5};
    Queue q{test, 5};
    //std::cout << q.is_empty() << std::endl;
    int dummy = 0;
    for (std::size_t i=5; i < 10; ++i) {
        dummy = q.extract_min();
        //std::cout << dummy << " " << q.size << " " << q.free_slots << std::endl;
    }
    std::cout << q.is_empty() << std::endl;
    //for (std::size_t i=0; i < 10; ++i) {
    //    q.dequeue();
    //    std:: cout << "###" << std::endl;
    //    for (std::size_t i=0; i < 10; ++i) std::cout << q[i] << std::endl;
    //}
    return 0;
}
