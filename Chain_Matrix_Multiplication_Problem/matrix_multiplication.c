#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"

#define MAX_VALUE 500  // maximum possible dimension
#define N_REPETITIONS 3  // number of repetititions for the timings
#define BILLION 1E9
// since the 'm' and 's' matrices of the algorithm are half-filled,
// we can optimize memory consumption by compacting the rows to the left;
// this macro returns the "compacted" index corresponding to the
// original row, col
#define IDX(mat, row, col) mat[row][col - row]


/**
  * Main matrix parenthetization routine,
  * iterative version. 'dim' is the array
  * of dimensions, whose length is 'n'
  */
size_t** matrix_chain(size_t* dim, size_t n) {
    // initialize matrices
    size_t** m = (size_t**)malloc(sizeof(size_t*) * n);
    size_t** s = (size_t**)malloc(sizeof(size_t*) * n);
    for (size_t i = 0; i < n; ++i) {
        // initialize the rows
        m[i] = (size_t*)malloc(sizeof(size_t) * (n - i));
        s[i] = (size_t*)malloc(sizeof(size_t) * (n - i));
        // since a chain with one matrix has cost 0, such will be the main diagonal
        IDX(m, i, i) = 0;
        // since a chain with only one matrix will have optimal parenthetization
        // the index of the matrix itself
        IDX(s, i, i) = i;
    }
    // for each diagonal
    for (size_t l = 1; l < n; ++l) {
        // for each row of the diagonal
        for (size_t i = 0; i < n - l; ++i) {
            size_t j = l + i;  // column index
            IDX(m, i, j) = IDX(m, i, j - 1) + IDX(m, j, j) + dim[i] * dim[j + 1] * dim[j];
            IDX(s, i, j) = j - 1;
            // perform the dynamic programming step
            for (size_t k = i + 1; k < j; ++k) {
                size_t c = IDX(m, i, k) + IDX(m, k + 1, j) + dim[i] * dim[j + 1] * dim[k + 1];
                if (c < IDX(m, i, j)) {
                    IDX(m, i, j) = c;
                    IDX(s, i, j) = k;
                }
            }
        }
    }
    // deallocate and return the s matrix
    for (size_t i = 0; i < n; i++) {
        free(m[i]);
    }
    free(m);
    return s;
}

/**
  * This function trivially copies an input matrix 'As' with 'a' rows
  * and 'b' columns.
  */
float** copy_matrix(float** As, size_t a, size_t b) {
    float** res = allocate_matrix(a, b);
    for (size_t i=0; i < a; ++i) {
        for (size_t j=0; j < b; ++j) {
            res[i][j] = As[i][j];
        }
    }
    return res;
}

/** Allocate and return an array of matrices with dimensions
  * dictated by 'dims', whose size is 'n'
  */
float*** build_problem_instance(size_t *dims, const size_t n) {
    float*** A = (float***)malloc(sizeof(float **) * n);
    for (size_t i=0; i < n; ++i) {
        A[i] = allocate_matrix(dims[i], dims[i + 1]);
    }
    return A;
}

/** Return an array of dimensions, sampled randomly
  * according to MAX_VALUE up to 'n' values
  */
size_t* build_dimensions(const size_t n) {
    size_t *dims = (size_t*)malloc(sizeof(size_t) * (n + 1));
    for (size_t i=0; i < n + 1; ++i) {
        dims[i] = rand() % MAX_VALUE;  // choose randomly number up to MAX_VALUE
    }
    return dims;
}

/**
  * Compute the matrix product among the 'As' matrices, having dimensions 'dims', whose length is 'n'.
  * 'S' should be the output of the optimal parenthetization algorithm, while 'i' and 'j' tell us
  * which subchain of matrices to consider. This problem is easily solved in a recursive fashion.
  */
float** evaluate_CMM_aux(float ***As, size_t *dims, const size_t n, size_t **S, size_t i, size_t j) {
    // if there is only one matrix, return it (by copying)
    if (i == j) {
        return copy_matrix(As[i], dims[i], dims[j + 1]);
    }
    // optimal parenthetization for the subchain i,..., j
    size_t p = IDX(S, i, j);
    // optimal parenthetization for the subchain i, ..., p
    float** C1 = evaluate_CMM_aux(As, dims, n, S, i, p);
    // optimal parenthetization for the subchain p + 1, ..., j
    float** C2 = evaluate_CMM_aux(As, dims, n, S, p + 1, j);
    // allocate matrix for result, whose dimension is trivially ixj+1
    float** R = allocate_matrix(dims[i], dims[j + 1]);
    // perform naive matrix multiplication
    naive_matrix_mult(R, C1, C2, dims[i], dims[p + 1], dims[p + 1], dims[j + 1]);
    // deallocate and return result
    deallocate_matrix(C1, dims[i]);
    deallocate_matrix(C2, dims[p + 1]);
    return R;
}

/**
  * Wrapper function to compute the matrix product among the matrices
  * 'As' with dimensions 'dims', with optimal parenthetization plan 'S'.
  * 'n' is the length of the 'dims' array.
  */
float **evaluate_CMM(float ***As, size_t *dims, const size_t n, size_t **S) {
    // return the main routine, with subchain from 0 to n - 1
    return evaluate_CMM_aux(As, dims, n, S, 0, n - 1);
}

/**
  * This function simply performs the same task as 'evaluate_CMM', but using
  * the naive algorithm that incrementally multiplies one matrix after the other,
  * starting from the first one
  */
float** evaluate_naive_CMM(float ***As, size_t *dims, const size_t n) {
  float** C = copy_matrix(As[0], dims[0], dims[1]);
  float** R;
  for (size_t i = 1; i < n; i++) {
      R = allocate_matrix(dims[0], dims[i + 1]);
      naive_matrix_mult(R, C, As[i], dims[0], dims[i], dims[i], dims[i + 1]);
      deallocate_matrix(C, dims[0]);
      C = R;
  }
  return C;
}

int main() {
  //number of total matrices
  size_t n = 10;
  // initialize the dimensions
  size_t *dims = build_dimensions(n);
  // allocate matrices
  float ***As = build_problem_instance(dims, n);
  // time facilities
  struct timespec start, end;
  double accum;

  printf("Input Size\tOptimal Solution\tNaive Solution\n");
  for (size_t d = 1; d < n; d++) {
      printf("%ld", d);
      clock_gettime(CLOCK_REALTIME, &requestStart);
      // evaluate performance for dynamic programming algorithm
      for (int r = 0; r < N_REPETITIONS; r++) {
          size_t **S = matrix_chain(dims, d);
          float **R = evaluate_CMM(As, dims, d, S);
          deallocate_matrix(R, dims[0]);
          deallocate_matrix((float**)S, d);
      }
      clock_gettime(CLOCK_REALTIME, &end);

      accum = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

      printf("\t%lf", (accum / N_REPETITIONS));
      clock_gettime(CLOCK_REALTIME, &start);
      // evaluate performance for the naive algorithm
      for (int r = 0; r < N_REPETITIONS; ++r) {
          float **R = evaluate_naive_CMM(As, dims, d);
          deallocate_matrix(R, dims[0]);
      }
      clock_gettime(CLOCK_REALTIME, &end);

      accum = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

      printf("\t%lf\n", (accum / N_REPETITIONS));
  }
  // deallocate
  for (size_t i = 0; i < n; i++) {
      deallocate_matrix(As[i], dims[i]);
  }
  free(As);
  free(dims);
  return 0;
}
