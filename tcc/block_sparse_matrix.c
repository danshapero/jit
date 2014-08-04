#include "block_sparse_matrix.h"
#include <stdlib.h>


void blockify(BlockSparseMatrix *A, SparseMatrix *B, int mc, int nc) {
    /* Set the global dimensions of the block sparse matrix */
    A->m = B->m;
    A->n = B->n;

    /* Set the block size of the matrix */
    A->mc = mc;
    A->nc = nc;

    /* Had better be sure that mc, nc divide m and n respectively */
    int M = A->m / A->mc;
    int N = A->n / A->nc;

    /* Make a graph `g` representing the connectivity structure of the dense
       blocks, and fill it accordingly */
    Graph *g;
    g = (Graph *)malloc(sizeof(Graph));
    initGraph(g, M, N);

    int d, i, j, k, l, I, J, K, index;
    for (i = 0; i < B->m; i++) {
        I = i / mc;
        for (k = B->ptr[i]; k < B->ptr[i + 1]; k++) {
            j = B->node[k];
            J = j / nc;
            addEdge(g, I, J);
        }
    }

    /* Knowing the number of edges in the graph, we now know the number of
       non-zero entries in the matrix */
    int ne = g->ne;
    A->nnz = ne * A->mc * A->nc;

    /* Allocate space for the internal storage of `A`, and set everything
       to zero */
    A->ptr = (int *)malloc( (M + 1) * sizeof(int) );
    A->node = (int *)malloc( ne * sizeof(int) );
    A->val = (double *)malloc( A->nnz * sizeof(double) );

    for (I = 0; I < M; I++) {
        A->ptr[I] = 0;
    }

    for (K = 0; K < ne; K++) {
        A->node[K] = -1;
    }

    for (k = 0; k < A->nnz; k++) {
        A->val[k] = 0.0;
    }

    /* Fill the array `ptr` according to how many non-zero blocks there are
       in each block-row of the matrix */
    for (I = 0; I < M; I++) {
        d = getDegree(g, I);
        A->ptr[I + 1] = A->ptr[I] + d;
    }

    /* Now fill in the structure of the matrix from the graph */
    d = getMaxDegree(g);
    int *neighbors;
    neighbors = (int *) malloc( d * sizeof(int) );
    for (K = 0; K < d; K++) neighbors[K] = 0;

    for (I = 0; I < M; I++) {
        d = getDegree(g, I);
        getNeighbors(g, neighbors, I);

        for (K = 0; K < d; K++) {
            J = neighbors[K];

            index = A->ptr[I];
            while(A->node[index] != -1) index++;
            A->node[index] = J;
        }
    }

    /* Fill the entries of `A` from those of `B` */
    for (I = 0; I < M; I++) {
        for (K = A->ptr[I]; K < A->ptr[I + 1]; K++) {
            J = A->node[K];

            for (k = 0; k < A->mc; k++) {
                i = I * A->mc + k;
                for (l = 0; l < A->nc; l++) {
                    j = J * A->nc + l;

                    index = (K * A->mc + k) * A->nc + l;
                    A->val[index] = getValue(B, i, j);
                }
            }
        }
    }

    /* Free some memory */
    free(neighbors);
    destroyGraph(g);
    free(g);
}


void destroyBlockSparseMatrix(BlockSparseMatrix *A) {
    A->m = 0;
    A->n = 0;
    A->mc = 0;
    A->nc = 0;
    A->nnz = 0;

    free( A->ptr );
    free( A->node );
    free( A->val );
}


void blockSparseMatSetValue(BlockSparseMatrix *A, int i, int j, double z) {
    int I = i / A->mc;
    int J = j / A->nc;
    int k = i % A->mc;
    int l = j % A->nc;

    int index;
    for (int K = A->ptr[I]; K < A->ptr[I + 1]; K++) {
        if (A->node[K] == J) {
            index = (K * A->mc + k) * A->nc + l;
            A->val[index] = z;
        }
    }
}


void blockSparseMatAddValue(BlockSparseMatrix *A, int i, int j, double z) {
    int I = i / A->mc;
    int J = j / A->nc;
    int k = i % A->mc;
    int l = j % A->nc;

    int index;
    for (int K = A->ptr[I]; K < A->ptr[I + 1]; K++) {
        if (A->node[K] == J) {
            index = (K * A->mc + k) * A->nc + l;
            A->val[index] += z;
        }
    }
}


double blockSparseMatGetValue(BlockSparseMatrix *A, int i, int j) {
    int index;
    double z = 0.0;

    int I = i / A->mc;
    int J = j / A->nc;
    int k = i % A->mc;
    int l = j % A->nc;

    for (int K = A->ptr[I]; K < A->ptr[I + 1]; K++) {
        if (A->node[K] == J) {
            index = (K * A->mc + k) * A->nc + l;
            z = A->val[index];
        }
    }

    return z;
}


int blockSparseMatGetRowSize(BlockSparseMatrix *A, int i) {
    int I = i / A->mc;
    return A->nc * (A->ptr[I + 1] - A->ptr[I]);
}


void blockSparseMatrixVectorMultiply(BlockSparseMatrix *A, double *x, double *y) {
    A->matvec(A, x, y);
}


void native_bcsr_matvec(BlockSparseMatrix *A, double *x, double *y) {
    int I, J, K, i, j, k, l, M, N, index;
    double z;

    M = A->m / A->mc;
    for (I = 0; I < M; I++) {
        for (K = A->ptr[I]; K < A->ptr[I + 1]; K++) {
            J = A->node[K];

            for (k = 0; k < A->mc; k++) {
                i = A->mc * I + k;
                for (l = 0; l < A->nc; l++) {
                    j = A->nc * J + l;
                    index = (K * A->mc + k) * A->nc + l;

                    y[i] += A->val[index] * x[j];
                }
            }
        }
    }
}

