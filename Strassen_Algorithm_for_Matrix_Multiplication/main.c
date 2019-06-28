#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "matrix.h"
#include "strassen.h"

#define MAX_ELEM_VALUE 25  // max possible value for a matrix element

/**
  * This function returns the seconds elapsed since the
  * start of the Unix epoch on 1 January, 1970
  */
double seconds() {
    struct timeval tmp;
    double sec;
    gettimeofday(&tmp, (struct timezone *)0);
    sec = tmp.tv_sec + ((double)tmp.tv_usec) / 1000000.0;
    return sec;
}
/**
  * Fill a matrix 'A' with random values. The matrix is of size
  * 'A_rows'x'A_cols'
  */
void randomly_fill_matrix(float **A, const size_t A_rows, const size_t A_cols) {
     for (size_t i=0; i< A_rows; i++) {
         for (size_t j=0; j< A_cols; j++) {
   	         A[i][j]=rand()%(2*MAX_ELEM_VALUE)-MAX_ELEM_VALUE;
         }
     }
}

/**
  * Return the wallclock time, given a start-time 'b_time'
  * and an end-time 'e_time'
  */
double get_execution_time(const struct timespec b_time, const struct timespec e_time) {
    return (e_time.tv_sec-b_time.tv_sec) + (e_time.tv_nsec-b_time.tv_nsec) / 1E9;
}

int main() {

  const size_t n=1000;
  // matrices
  float **C0=allocate_matrix(n,n);  // output for improved Strassen's algorithm
  float **C1=allocate_matrix(n,n);  // output for original Strassen's algorithm
  // the two matrices to multiply
  float **A=allocate_matrix(n,n);
  float **B=allocate_matrix(n,n);
  // random seed
  srand(10);
  // fill and time utilities
  randomly_fill_matrix(A, n, n);
  randomly_fill_matrix(B, n, n);

  // perform measurements
  for (size_t i=1; i<=n; i*=2) {
      printf("%ld", i);

      double start = seconds();
      improved_strassen2(C0, A, B, i);
      double end = seconds();

      printf("\t%f", end - start);

      start = seconds();
      improved_strassen(C0, A, B, i);
      end = seconds();

      printf("\t%f", end - start);

      start = seconds();
      strassen(C1, A, B, i);
      end = seconds();

      printf("\t%f", end - start);
      // check correctness
      printf("\t%d\n", same_matrix(C0, i, i, C1, i, i));
}
  // deallocate
  deallocate_matrix(C0,n);
  deallocate_matrix(C1,n);
  deallocate_matrix(A,n);
  deallocate_matrix(B,n);
  return 0;
}
