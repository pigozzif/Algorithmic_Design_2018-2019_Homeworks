#include <stdio.h>
#include <stdlib.h>
#include <math.h>    // for INFINITY
#include <time.h>
#include "matrix.h"

#define UPPER 10   // upper bound for matrix dimensions
#define LOWER 1   // lower bound for matrix dimensions
#define MAX_VALUE 50

// auxiliary function, performs dynamic programming step

void matrix_chain_aux(int* P, float** m, float** s, int i, int j) {
    m[i][j] = INFINITY;
    for (int k=i; k < j; ++k) {
        float q = m[i][k] + m[k+1][j] + P[i]*P[k+1]*P[j+1];
        if (q < m[i][j]) {
            m[i][j] = q;
            s[i][j-1] = k + 1;
        }
    }
}

// main matrix parenthetization routine, iterative version

float*** matrix_chain(int* P, int n) {
    float** m = allocate_matrix(n - 1, n - 1);
    float** s = allocate_matrix(n - 2, n - 2);
    for (int i=0; i < n - 1; ++i) m[i][i] = 0.0;
    for (int l=2; l < n; ++l) {
        for (int i=0; i < n - l; ++i) {
            int j = i + l - 1;
            matrix_chain_aux(P, m, s, i, j);
        }
    }
    // Let's return m and s in an array of two elements
    float*** ans = (float ***)malloc(sizeof(float**) * 2);
    ans[0] = m;
    ans[1] = s;
    return ans;
}

int main(int argc, char *argv[]) {
    return 0;
}
