#include "graph.h"

#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

struct SparseMatrix_t {
    int m, n, nnz;
    int *ptr, *node;
    double *val;
    void (*matvec) (struct SparseMatrix_t *A, double *x, double *y);
};

typedef struct SparseMatrix_t SparseMatrix;
typedef void (*Matvec) (SparseMatrix *A, double *x, double *y);

void initSparseMatrix(SparseMatrix *A, int m, int n, Graph *g);
void destroySparseMatrix(SparseMatrix *A);

void setValue(SparseMatrix *A, int i, int j, double z);
void addValue(SparseMatrix *A, int i, int j, double z);

double getValue(SparseMatrix *A, int i, int j);
int getRowSize(SparseMatrix *A, int i);

void sparseMatrixVectorMultiply(SparseMatrix *A, double *x, double *y);
void native_csr_matvec(SparseMatrix *A, double *x, double *y);

#endif
