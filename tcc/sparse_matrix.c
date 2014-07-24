#include <stdlib.h>
#include "sparse_matrix.h"
#include <stdio.h>


void initSparseMatrix(SparseMatrix *A, int m, int n, Graph *g) {
    A->m = m;
    A->n = n;
    A->nnz = g->ne;

    /* Allocate space for the internal storage of `A` */
    A->ptr = (int *) malloc( (m + 1) * sizeof(int) );
    A->node = (int *) malloc( A->nnz * sizeof(int) );
    A->val = (double *) malloc( A->nnz * sizeof(double) );

    int i, j, k, d, index;

    /* Set all the storage structures in `A` to those of an empty matrix */
    for (i = 0; i < m + 1; i++) {
        A->ptr[i] = 0;
    }

    for (k = 0; k < A->nnz; k++) {
        A->node[k] = -1;
        A->val[k] = 0.0;
    }

    /* Fill the array `ptr` according to how many non-zero entries there are in
       each row of the matrix */
    for (i = 0; i < m; i++) {
        d = getDegree(g, i);
        A->ptr[i + 1] = A->ptr[i] + d;
    }

    /* Fill the structure of the matrix from the graph */
    d = getMaxDegree(g);
    int *neighbors;
    neighbors = (int *) malloc( d * sizeof(int) );
    for (k = 0; k < d; k++) neighbors[k] = 0;

    for (i = 0; i < m; i++) {

        d = getDegree(g, i);
        getNeighbors(g, neighbors, i);

        for (k = 0; k < d; k++) {
            j = neighbors[k];

            index = A->ptr[i];
            while(A->node[index] != -1) index++;
            A->node[index] = j;
        }
    }

    free(neighbors);

    /* Make the matrix-vector multiply implementation point to a CSR matvec
       algorithm */
    A->matvec = native_csr_matvec;
}


void destroySparseMatrix(SparseMatrix *A) {
    A->m = 0;
    A->n = 0;
    A->nnz = 0;

    free( A->ptr );
    free( A->node );
    free( A->val );
}


void setValue(SparseMatrix *A, int i, int j, double z) {
    for (int k = A->ptr[i]; k < A->ptr[i + 1]; k++) {
        if (A->node[k] == j) A->val[k] = z;
    }
}


void addValue(SparseMatrix *A, int i, int j, double z) {
    for (int k = A->ptr[i]; k < A->ptr[i + 1]; k++) {
        if (A->node[k] == j) A->val[k] += z;
    }
}


double getValue(SparseMatrix *A, int i, int j) {
    double z = 0.0;

    for (int k = A->ptr[i]; k < A->ptr[i + 1]; k++) {
        if (A->node[k] == j) z = A->val[k];
    }

    return z;
}


int getRowSize(SparseMatrix* A, int i) {
    return A->ptr[i + 1] - A->ptr[i];
}


void sparseMatrixVectorMultiply(SparseMatrix *A, double *x, double *y) {
    A->matvec(A, x, y);
}


void native_csr_matvec(SparseMatrix *A, double *x, double *y) {
    int i, j, k;
    double z;

    for (i = 0; i < A->m; i++) {
        z = 0.0;

        for (k = A->ptr[i]; k < A->ptr[i + 1]; k++) {
            j = A->node[k];
            z += A->val[k] * x[j];
        }

        y[i] = z;
    }

}

