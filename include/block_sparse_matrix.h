#include "sparse_matrix.h"

#ifndef BLOCK_SPARSE_MATRIX_H
#define BLOCK_SPARSE_MATRIX_H


typedef void (*BlockMatvec) (int m, int n, int mc, int nc, int nnz,
                             int *ptr, int *node, double *val,
                             double *x, double *y);

struct BlockSparseMatrix_t {
    int m, n, mc, nc, nnz;
    int *ptr, *node;
    double *val;
    BlockMatvec matvec;
};

typedef struct BlockSparseMatrix_t BlockSparseMatrix;

void blockify(BlockSparseMatrix *A, SparseMatrix *B, int mc, int nc);
void destroyBlockSparseMatrix(BlockSparseMatrix *A);

void blockSparseMatSetValue(BlockSparseMatrix *A, int i, int j, double z);
void blockSparseMatAddValue(BlockSparseMatrix *A, int i, int j, double z);

double blockSparseMatGetValue(BlockSparseMatrix *A, int i, int j);
int blockSparseMatGetRowSize(BlockSparseMatrix *A, int i);

void blockSparseMatrixVectorMultiply(BlockSparseMatrix *A, double *x, double *y);
void native_bcsr_matvec(int m, int n, int mc, int nc, int nnz,
                        int *ptr, int *node, double *val,
                        double *x, double *y);

#endif
