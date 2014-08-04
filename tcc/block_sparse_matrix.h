#include "sparse_matrix.h"

struct BlockSparseMatrix_t {
    int m, n, mc, nc, nnz;
    int *ptr, *node;
    double *val;
    void (*matvec) (struct BlockSparseMatrix_t *A, double *x, double *y);
};

typedef struct BlockSparseMatrix_t BlockSparseMatrix;
typedef void (*BlockMatvec) (BlockSparseMatrix *A, double *x, double *y);

void blockify(BlockSparseMatrix *A, SparseMatrix *B, int mc, int nc);
void destroyBlockSparseMatrix(BlockSparseMatrix *A);

void blockSparseMatSetValue(BlockSparseMatrix *A, int i, int j, double z);
void blockSparseMatAddValue(BlockSparseMatrix *A, int i, int j, double z);

double blockSparseMatGetValue(BlockSparseMatrix *A, int i, int j);
int blockSparseMatGetRowSize(BlockSparseMatrix *A, int i);

void blockSparseMatrixVectorMultiply(BlockSparseMatrix *A, double *x, double *y);
void native_bcsr_matvec(BlockSparseMatrix *A, double *x, double *y);
