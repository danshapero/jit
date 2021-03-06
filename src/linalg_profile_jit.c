
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

    jitCompileUnrolledBlockMatvec(CompilerState, "ubcsr_matvec", nb, nb);
    jitCompileMatvec(CompilerState, "rcsr_matvec");

    /* Relocate all the JIT compiled code into executable memory */
    if (tcc_relocate(CompilerState) < 0) {
        printf("Unable to relocate JIT code! Oh no!\n");
        return 1;
    }

    BlockMatvec amv;
    amv = jitGetBlockMatvec(CompilerState, "ubcsr_matvec");
    Matvec bmv;
    bmv = jitGetMatvec(CompilerState, "rcsr_matvec");

    printf("Memory location of block matvec: %x\n", (int)amv);
    printf("Memory location of csr matvec:   %x\n", (int)bmv);

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


    clock_t start, end;
    start = clock();
    for (k = 0; k < nn; k++) {
        blockSparseMatrixVectorMultiply(A, x, y);
    }
    end = clock();
    double elapsed = (end - start) * 1000.0 / CLOCKS_PER_SEC;

    printf("Block sparse matrix-vector multiply: %g\n", elapsed);


    start = clock();
    for (k = 0; k < nn; k++) {
        sparseMatrixVectorMultiply(B, x, y);
    }
    end = clock();
    elapsed = (end - start) * 1000.0 / CLOCKS_PER_SEC;

    printf("Sparse matrix-vector multiply:       %g\n", elapsed);


    /* Free up heap-allocated memory */

    free(x);
    free(y);

    tcc_delete(CompilerState);

    destroyBlockSparseMatrix(A);
    free(A);

    destroySparseMatrix(B);
    free(B);

    return 0;
}
