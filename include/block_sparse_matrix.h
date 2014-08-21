#include "sparse_matrix.h"

#ifndef BLOCK_SPARSE_MATRIX_H
#define BLOCK_SPARSE_MATRIX_H


typedef struct BlockSparseMatrix {
    int m, n, mc, nc, nnz;
    int *ptr, *node;
    double *val;
    void (*matvec) (struct BlockSparseMatrix*, double*, double*);
} BlockSparseMatrix;

typedef void (*BlockMatvec) (BlockSparseMatrix*, double*, double*);


void blockify(BlockSparseMatrix *A, SparseMatrix *B, int mc, int nc);
void destroyBlockSparseMatrix(BlockSparseMatrix *A);

void blockSparseMatSetValue(BlockSparseMatrix *A, int i, int j, double z);
void blockSparseMatAddValue(BlockSparseMatrix *A, int i, int j, double z);

double blockSparseMatGetValue(BlockSparseMatrix *A, int i, int j);
int blockSparseMatGetRowSize(BlockSparseMatrix *A, int i);

void blockSparseMatrixVectorMultiply(BlockSparseMatrix *A, double *x, double *y);
void native_bcsr_matvec(BlockSparseMatrix *A, double *x, double *y);

#endif
