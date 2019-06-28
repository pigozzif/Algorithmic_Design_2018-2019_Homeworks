
/** A first version of an improved algorithm has been achieved by reducing to
  * 1/7th of the total the space for the matrices P. In fact, we can initially (after the
  * computation of the S matrices) store four of the 7 Ps each in one of the Cs. Then,
  * use a smart sequence of addititions/subtractions among the Cs and auxiliary matrix
  * (as big as a generic P) to complete the remaining Ps one after the other.
  * Subsequently, another improved version of the algorithm is offered, in which we apply
  * the same reasoning of before to the S matrices, instead of the Ps. In particular, it
  * can be noticed from the pseudocode of the algorithm that we need only one or two Ss at
  * a time (that is, to compute a given P). In this way, we move from the original 10 Ss
  * to only 2. For what concerns speedup, both versions showed an improved over the naive
  * one, but the decrease in time was more pronounced for the second version. According to me,
  * the reason for this is that the second version reduces the number of S matrices by 8, against
  * the 6 Ps eliminated by the first (S and P matrices have the same dimension within the same
  * recursive call). Moreover, the first version needs to compute a lot more summations and subtractions
  * to get to the final Cs, increasing also how much the algorithm is compute-bound.
  */

#include <stdlib.h>

#include "matrix.h"
#include "strassen.h"

/**
  * Auxiliary function to perform the Strassen's matrix multiplication algorithm,
  * using a version that has been optimized for auxiliary memory
  */
void improved_strassen_aux(float **C, const size_t C_f_row, const size_t C_f_col,
               float **A, const size_t A_f_row, const size_t A_f_col,
               float **B, const size_t B_f_row, const size_t B_f_col,
               const size_t n) {
  // if the size of the matrix is not really big, we can trigger the base case
  // of the recursion and perform a naive matrix multiplication
  if (n < (1<<6)) {
    naive_aux(C, C_f_row, C_f_col,
              A, A_f_row, A_f_col,
              B, B_f_row, B_f_col,
              n);
    return;
  }

  const size_t n2=n/2;  // half the size of the matrix
  // necessary offsets
  const size_t C1X = C_f_row;
  const size_t C2X = C_f_row + n2;
  const size_t CX1 = C_f_col;
  const size_t CX2 = C_f_col + n2;

  const size_t A1X = A_f_row;
  const size_t A2X = A_f_row + n2;
  const size_t AX1 = A_f_col;
  const size_t AX2 = A_f_col + n2;

  const size_t B1X = B_f_row;
  const size_t B2X = B_f_row + n2;
  const size_t BX1 = B_f_col;
  const size_t BX2 = B_f_col + n2;
  // allocate an array for the S matrices, as named in the
  // pseudocode. Each one has size half of the matrices under discussion
  float*** S=(float***)malloc(sizeof(float**) * 10);
  for (int i=0; i < 10; ++i) {
      S[i] = allocate_matrix(n2, n2);
  }
  // we now allocate one and only one P matrix, down from the
  // original 7
  float **aux = allocate_matrix(n2, n2);

  // S1 = B12 - B22
  sub_matrix_blocks(S[0],0,0,
                    B,B1X,BX2,
                    B,B2X,BX2,
                    n2);

  // S2 = A11 + A12
  sum_matrix_blocks(S[1],0,0,
                    A,A1X,AX1,
                    A,A1X,AX2,
                    n2);

  // S3 = A21 + A22
  sum_matrix_blocks(S[2],0,0,
                    A,A2X,AX1,
                    A,A2X,AX2,
                    n2);

  // S4 = B21 - B11
  sub_matrix_blocks(S[3],0,0,
                    B,B2X,BX1,
                    B,B1X,BX1,
                    n2);

  // S5 = A11 + A22
  sum_matrix_blocks(S[4],0,0,
                    A,A1X,AX1,
                    A,A2X,AX2,
                    n2);

  // S6 = B11 + B22
  sum_matrix_blocks(S[5],0,0,
                    B,B1X,BX1,
                    B,B2X,BX2,
                    n2);

  // S7 = A12 - A22
  sub_matrix_blocks(S[6],0,0,
                    A,A1X,AX2,
                    A,A2X,AX2,
                    n2);
  // S8 = B21 + B22
  sum_matrix_blocks(S[7],0,0,
                    B,B2X,BX1,
                    B,B2X,BX2,
                    n2);

  // S9 = A11 - A21
  sub_matrix_blocks(S[8],0,0,
                    A,A1X,AX1,
                    A,A2X,AX1,
                    n2);

  // S10 = B11 + B12
  sum_matrix_blocks(S[9],0,0,
                    B,B1X,BX1,
                    B,B1X,BX2,
                    n2);


  // P2 = S2 x B22, can be saved in C12
  improved_strassen_aux(C, C1X, CX2,
               S[1], 0, 0,
               B, B2X, BX2, n2);
  // P4 = A22 x S4, can be saved in C21
  improved_strassen_aux(C, C2X, CX1,
               A, A2X, AX2,
               S[3], 0, 0, n2);

  // P5 = S5 x S6, can be saved in C22
  improved_strassen_aux(C, C2X, CX2,
               S[4], 0, 0,
               S[5], 0, 0, n2);

  // P6 = S7 x S8, can be saved in C11
  improved_strassen_aux(C, C1X, CX1,
               S[6], 0, 0,
               S[7], 0, 0, n2);

  // C11 = (P5 + P4) - P2 + P6. Will be obtained
  // by summing the other three Cs (which are: P2,
  // P4 and P5) to the existing C11, where P6 is stored
  sum_matrix_blocks(C, C1X, CX1,
                    C, C1X, CX1,
                    C, C2X, CX1, n2);
  sum_matrix_blocks(C, C1X, CX1,
                    C, C1X, CX1,
                    C, C2X, CX2, n2);
  sub_matrix_blocks(C, C1X, CX1,
                    C, C1X, CX1,
                    C, C1X, CX2, n2);

  // P3 = S3 x B11; here is where the auxiliary
  // matrix kicks-in
  improved_strassen_aux(aux, 0, 0,
                        S[2], 0, 0,
                        B, B1X, BX1, n2);

  // C21 = P3 + P4, by means of aux (where P3 is stored);
  // P4 is already "on-site"
  sum_matrix_blocks(C, C2X, CX1,
                    C, C2X, CX1,
                    aux, 0, 0, n2);
  // In the following, we also update C22, which depends on
  // P3 through subtraction (it is stored in aux)
  sub_matrix_blocks(C, C2X, CX2,
                    C, C2X, CX2,
                    aux, 0, 0, n2);

  // P1 = A11 x S1, overwrite on aux
  improved_strassen_aux(aux, 0, 0,
               A, A1X, AX1,
               S[0], 0, 0, n2);

  // C12 = P1 + P2, by means of aux (P1), while P2 is "on-site"
  sum_matrix_blocks(C, C1X, CX2,
                    C, C1X, CX2,
                    aux, 0, 0, n2);
  // In the following, we also update C22, which depends on
  // P1 through summation (it is stored in aux)
  sum_matrix_blocks(C, C2X, CX2,
                    C, C2X, CX2,
                    aux, 0, 0, n2);

  // P7 = S9 x S10, overwrite on aux
  improved_strassen_aux(aux, 0, 0,
                        S[8], 0, 0,
                        S[9], 0, 0, n2);

  // C22 = P5 + P1 - P3 - P7, by means of aux (P7),
  // while the rest is already "on-site"
  sub_matrix_blocks(C, C2X, CX2,
                    C, C2X, CX2,
                    aux, 0, 0,
                    n2);

  // deallocate S matrices
  for (int i=0; i<10; i++) {
      deallocate_matrix(S[i], n2);
  }
  // deallocate aux
  deallocate_matrix(aux, n2);
}

/**
  * Wrapper function to perform the Strassen's improved matrix
  * multiplication algorithm on matrices 'A' and 'B', storing
  * the result in 'C', and applying the required offsets to the
  * auxiliary function
  */
void improved_strassen(float **C,
      float **A, float **B, const size_t n) {
    improved_strassen_aux(C, 0, 0, A, 0, 0, B, 0, 0, n);
}

/**
  * Auxiliary function to perform the Strassen's matrix multiplication algorithm,
  * using an alternative to the version that has been optimized for auxiliary memory.
  * Here the focus of optimization are the S matrices, not the P ones
  */
void improved_strassen_aux2(float **C, const size_t C_f_row, const size_t C_f_col,
               float **A, const size_t A_f_row, const size_t A_f_col,
               float **B, const size_t B_f_row, const size_t B_f_col,
               const size_t n) {
  // if the size of the matrix is not really big, we can trigger the base case
  // of the recursion and perform a naive matrix multiplication
  if (n < (1<<6)) {
    naive_aux(C, C_f_row, C_f_col,
              A, A_f_row, A_f_col,
              B, B_f_row, B_f_col,
              n);
    return;
  }

  const size_t n2=n/2;  // half the size of the matrix
  // necessary offsets
  const size_t C1X = C_f_row;
  const size_t C2X = C_f_row + n2;
  const size_t CX1 = C_f_col;
  const size_t CX2 = C_f_col + n2;

  const size_t A1X = A_f_row;
  const size_t A2X = A_f_row + n2;
  const size_t AX1 = A_f_col;
  const size_t AX2 = A_f_col + n2;

  const size_t B1X = B_f_row;
  const size_t B2X = B_f_row + n2;
  const size_t BX1 = B_f_col;
  const size_t BX2 = B_f_col + n2;
  // allocate an array for the S matrices, as named in the
  // pseudocode. Each one has size half of the matrices under discussion
  // notice we have only two of them here
  float*** S=(float***)malloc(sizeof(float**) * 10);
  for (int i=0; i < 2; ++i) {
      S[i] = allocate_matrix(n2, n2);
  }
  // allocate the array of the P matrices
  float*** P=(float***)malloc(sizeof(float**) * 7);
  for (int i=0; i < 7; ++i) {
      P[i] = allocate_matrix(n2, n2);
  }

  // S1 = B12 - B22
  sub_matrix_blocks(S[0],0,0,
                    B,B1X,BX2,
                    B,B2X,BX2,
                    n2);
  // S2 = A11 + A12
  sum_matrix_blocks(S[1],0,0,
                    A,A1X,AX1,
                    A,A1X,AX2,
                    n2);

  // P1 = A11 x S1
  improved_strassen_aux2(P[0], 0, 0,
               A, A1X, AX1,
               S[0], 0, 0,
               n2);

  // P2 = S2 x B22
  improved_strassen_aux2(P[1], 0, 0,
               S[1], 0, 0,
               B, B2X, BX2,
               n2);

  // S3 = A21 + A22
  sum_matrix_blocks(S[0],0,0,
                    A,A2X,AX1,
                    A,A2X,AX2,
                    n2);

  // P3 = S3 x B11
  improved_strassen_aux2(P[2], 0, 0,
               S[0], 0, 0,
               B, B1X, BX1,
               n2);

  // S4 = B21 - B11
  sub_matrix_blocks(S[1],0,0,
                    B,B2X,BX1,
                    B,B1X,BX1,
                    n2);

  // P4 = A22 x S4
  improved_strassen_aux2(P[3], 0, 0,
               A, A2X, AX2,
               S[1], 0, 0,
               n2);

  // S5 = A11 + A22
  sum_matrix_blocks(S[0],0,0,
                    A,A1X,AX1,
                    A,A2X,AX2,
                    n2);

  // S6 = B11 + B22
  sum_matrix_blocks(S[1],0,0,
                    B,B1X,BX1,
                    B,B2X,BX2,
                    n2);

  // P5 = S5 x S6
  improved_strassen_aux2(P[4], 0, 0,
               S[0], 0, 0,
               S[1], 0, 0,
               n2);

  // S7 = A12 - A22
  sub_matrix_blocks(S[0],0,0,
                    A,A1X,AX2,
                    A,A2X,AX2,
                    n2);

  // S8 = B21 + B22
  sum_matrix_blocks(S[1],0,0,
                    B,B2X,BX1,
                    B,B2X,BX2,
                    n2);

  // P6 = S7 x S8
  improved_strassen_aux2(P[5], 0, 0,
               S[0], 0, 0,
               S[1], 0, 0,
               n2);

  // S9 = A11 - A21
  sub_matrix_blocks(S[0],0,0,
                    A,A1X,AX1,
                    A,A2X,AX1,
                    n2);

  // S10 = B11 + B12
  sum_matrix_blocks(S[1],0,0,
                    B,B1X,BX1,
                    B,B1X,BX2,
                    n2);

  // P7 = S9 x S10
  improved_strassen_aux2(P[6], 0, 0,
               S[0], 0, 0,
               S[1], 0, 0,
               n2);

  // C11 = (P5 + P4) - P2 + P6
  sum_matrix_blocks(C, C1X, CX1,
                    P[4], 0, 0,
                    P[3], 0, 0,
                    n2);

  sub_matrix_blocks(C, C1X, CX1,
                    C, C1X, CX1,
                    P[1], 0, 0,
                    n2);

  sum_matrix_blocks(C, C1X, CX1,
                    C, C1X, CX1,
                    P[5], 0, 0,
                    n2);

  // C12 = P1 + P2
  sum_matrix_blocks(C, C1X, CX2,
                    P[0], 0, 0,
                    P[1], 0, 0,
                    n2);

  // C21 = P3 + P4
  sum_matrix_blocks(C, C2X, CX1,
                    P[2], 0, 0,
                    P[3], 0, 0,
                    n2);

  // C22 = P5 + P1 - P3 - P7
  sum_matrix_blocks(C, C2X, CX2,
                    P[4], 0, 0,
                    P[0], 0, 0,
                    n2);

  sub_matrix_blocks(C, C2X, CX2,
                    C, C2X, CX2,
                    P[2], 0, 0,
                    n2);

  sub_matrix_blocks(C, C2X, CX2,
                    C, C2X, CX2,
                    P[6], 0, 0,
                    n2);

  // deallocate S matrices
  for (int i=0; i < 2; i++) {
      deallocate_matrix(S[i], n2);
  }
  // deallocate P matrices
  for (int i=0; i < 7; ++i) {
      deallocate_matrix(P[i], n2);
  }
}

/**
  * Wrapper function to perform the Strassen's improved matrix
  * multiplication algorithm (in the second version, the one with
  * less Ss) on matrices 'A' and 'B', storing the result in 'C',
  * and applying the required offsets to the auxiliary function
  */
void improved_strassen2(float **C,
      float **A, float **B, const size_t n) {
    improved_strassen_aux2(C, 0, 0, A, 0, 0, B, 0, 0, n);
}
