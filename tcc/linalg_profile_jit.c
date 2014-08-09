
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#include "jit_sparse_matrix_kernels.h"
#include "jit_block_sparse_matrix_kernels.h"



int main(int argc, char **argv) {

     /* -------------------- */
    /* Build a random graph */
   /* -------------------- */

    int i, j, k, I, J, mc, nc, nb, nn, N, ne;
    double p, q;

    nn = 512;


    Graph *h;
    h = (Graph *) malloc(sizeof(Graph));
    initGraph(h, nn, nn);

    srand(time(NULL));
    p = log(1.0 * nn) / nn;
    for (i = 0; i < nn; i++) {
        addEdge(h, i, i);

        for (j = i + 1; j < nn; j++) {
            q = (1.0 * rand()) / RAND_MAX;
            if (q < p) {
                addEdge(h, i, j);
                addEdge(h, j, i);
            }
        }
    }


    /* Build a new graph on `h` which has a natural block structure */
    Graph *g;
    g = (Graph *) malloc(sizeof(Graph));
    nb = 8;
    initGraph(g, nb * nn, nb * nn);

    int d, *neighbors;
    d = getMaxDegree(h);
    neighbors = (int *) malloc(d * sizeof(int));

    for (I = 0; I < nn; I++) {
        getNeighbors(h, neighbors, I);
        d = getDegree(h, I);

        for (k = 0; k < d; k++) {
            J = neighbors[k];

            for (i = nb * I; i < nb * (I + 1); i++) {
                for (j = nb * J; j < nb * (J + 1); j++) {
                    addEdge(g, i, j);
                }
            }
        }
    }

    destroyGraph(h);
    free(h);
    free(neighbors);



     /* ------------------------- */
    /* Build the graph Laplacian */
   /* ------------------------- */

    N = nb * nn;

    SparseMatrix *B;
    B = (SparseMatrix *) malloc( sizeof(SparseMatrix) );
    initSparseMatrix(B, N, N, g);

    d = getMaxDegree(g);
    neighbors = (int *) malloc(d * sizeof(int));

    for (i = 0; i < nn; i++) {
        getNeighbors(g, neighbors, i);
        d = getDegree(g, i);

        for (k = 0; k < d; k++) {
            j = neighbors[k];

            addValue(B, i, i, +1.0);
            addValue(B, i, j, -1.0);
        }
    }

    free(neighbors);

    destroyGraph(g);
    free(g);



     /* ------------------- */
    /* Blockify the matrix */
   /* ------------------- */

    BlockSparseMatrix *A;
    A = (BlockSparseMatrix *) malloc( sizeof(BlockSparseMatrix) );
    blockify(A, B, nb, nb);

    printf("Blockifying sparse matrix.\n");



     /* ------------------------------------------- */ 
    /* Compile and affix JIT sparse matrix kernels */
   /* ------------------------------------------- */

    TCCState *CompilerState;
    CompilerState = tcc_new();

    BlockMatvec amv;
    amv = jitCompileUnrolledBlockMatvec(CompilerState, nb, nb);

    Matvec bmv;
    bmv = jitCompileMatvec(CompilerState);

    A->matvec = amv;
    B->matvec = bmv;



     /* ------------ */
    /* It's a race! */
   /* ------------ */

    double *x, *y;

    x = (double *) malloc( N * sizeof(double) );
    y = (double *) malloc( N * sizeof(double) );

    for (i = 0; i < N; i++) {
        x[i] = (1.0 * rand()) / RAND_MAX;
        y[i] = 0.0;
    }


    clock_t start = clock(), diff;
    for (k = 0; k < nn; k++) {
        blockSparseMatrixVectorMultiply(A, x, y);
    }
    diff = clock() - start;

    int msec_A = diff * 1000 / CLOCKS_PER_SEC;


    start = clock();
    for (k = 0; k < nn; k++) {
        sparseMatrixVectorMultiply(B, x, y);
    }
    diff = clock() - start;

    int msec_B = diff * 1000 / CLOCKS_PER_SEC;


    printf("Block sparse matrix-vector multiply: %d ms \n", msec_A);
    printf("Sparse matrix-vector multiply:       %d ms \n", msec_B);



    /* Free up heap-allocated memory */

    tcc_delete(CompilerState);

    destroyBlockSparseMatrix(A);
    free(A);

    destroySparseMatrix(B);
    free(B);

    return 0;
}
