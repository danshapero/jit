#include "graph.h"

#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

typedef void (*Matvec) (int m, int n, int nnz,              // Matrix size
                        int *ptr, int *node, double *val,   // Matrix structure
                        double *x, double *y);              // Vectors

struct SparseMatrix_t {
    int m, n, nnz;
    int *ptr, *node;
    double *val;
    Matvec matvec;
};

typedef struct SparseMatrix_t SparseMatrix;

void initSparseMatrix(SparseMatrix *A, int m, int n, Graph *g);
void destroySparseMatrix(SparseMatrix *A);

void setValue(SparseMatrix *A, int i, int j, double z);
void addValue(SparseMatrix *A, int i, int j, double z);

double getValue(SparseMatrix *A, int i, int j);
int getRowSize(SparseMatrix *A, int i);

void sparseMatrixVectorMultiply(SparseMatrix *A, double *x, double *y);
void native_csr_matvec(int m, int n, int nnz,
                       int *ptr, int *node, double *val,
                       double *x, double *y);

#endif
