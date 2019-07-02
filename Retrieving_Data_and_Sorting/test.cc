#include <iostream>

void myfunc(int* A, int end) {
//    for (int i=0; i < end; ++i) std::cout << *(A + i) << std::endl;
    A[0] = 90;
}

int main() {
    int test[4] = {1, 2, 3, 4};
    myfunc(test + 1, 3);
    for (int i=0; i < 4; ++i) std::cout << test[i] << " ";
    return 0;
}
