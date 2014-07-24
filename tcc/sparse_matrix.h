#include "graph.h"


typedef struct {
    int m, n, nnz;
    int *ptr, *node;
    double *val;
} SparseMatrix;

void initSparseMatrix(SparseMatrix *A, int m, int n, Graph *g);
void destroySparseMatrix(SparseMatrix *A);

void setValue(SparseMatrix *A, int i, int j, double z);
void addValue(SparseMatrix *A, int i, int j, double z);

double getValue(SparseMatrix *A, int i, int j);
int getRowSize(SparseMatrix *A, int i);

void matvec(SparseMatrix *A, double *x, double *y);
