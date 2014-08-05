
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#include "jit_block_sparse_matrix_kernels.h"



int main(int arg, char **argv) {

    int i, j, k, nn, ne;
    double z, p;
    nn = 512;


    /* Build an Erdos-Renyi graph `g` */
    Graph *g;
    g = (Graph *) malloc(sizeof(Graph));
    initGraph(g, nn, nn);

    srand(time(NULL));
    p = log(1.0 * nn) / nn;
    for (i = 0; i < nn; i++) {
        addEdge(g, i, i);

        for (j = i + 1; j < nn; j++) {
            z = (1.0 * rand()) / RAND_MAX;
            if (z < p) {
                addEdge(g, i, j);
                addEdge(g, j, i);
            }
        }
    }

    printf("Done building Erdos-Renyi graph.\n");
    printf("    Vertices:   %d\n", nn);
    printf("    Edges:      %d\n", g->ne);
    printf("    Max degree: %d\n", getMaxDegree(g));


    /* Build a matrix on the graph `g` */
    SparseMatrix *B;
    B = malloc( sizeof(SparseMatrix) );
    initSparseMatrix(B, nn, nn, g);


    /* Construct the Laplacian of the graph `g` */
    int d = getMaxDegree(g);
    int *neighbors;
    neighbors = (int *) malloc( d * sizeof(int) );

    for (i = 0; i < nn; i++) {

        d = getDegree(g, i);
        getNeighbors(g, neighbors, i);

        for (k = 0; k < d; k++) {
            j = neighbors[k];

            addValue(B, i, i, +1.0);
            addValue(B, i, j, -1.0);
        }
    }

    free(neighbors);

    printf("\n");
    printf("Constructing graph Laplacian.\n");


    /* Turn the matrix into a block matrix */
    BlockSparseMatrix *A;
    A = malloc( sizeof(BlockSparseMatrix) );
    blockify(A, B, 8, 8);

    printf("Blockifying sparse matrix.\n");


    /* Make some vectors and multiply them by `A` */
    double *x, *y;
    x = (double *) malloc( nn * sizeof(double) );
    y = (double *) malloc( nn * sizeof(double) );
    for (i = 0; i < nn; i++) {
        x[i] = 1.0;
        y[i] = 1.0;
    }


    printf("Multiplying blockified graph Laplacian by constant vector 1.\n");
    blockSparseMatrixVectorMultiply(A, x, y);

    for (i = 0; i < nn; i++) {
        if (y[i] != 0.0) {
            printf("%d %g\n", i, y[i]);
            return 1;
        }
    }

    printf("Block matrix multiplication gave correct result of 0.\n");


    printf("JIT compiling the block matrix-vector multiplication kernel:\n");
    BlockMatvec mv;
    mv = jitCompileBlockMatvec(8, 8);
    printf("Done JIT compiling matvec kernel!\n");

    printf("Location in memory of native block matvec:  %d\n",
                                            (int)(&native_bcsr_matvec));
    printf("Location in memory of JIT block matvec:     %d\n", (int)mv);


    printf("Changing A's implementation of block matvec to point to \n");
    printf("    the JIT-compiled matvec.\n");
    A->matvec = mv;
    printf("New location of A's block matvec implementation: %d\n",
                                                        (int)A->matvec);

    for (i = 0; i < nn; i++) {
        x[i] = 1.0;
        y[i] = 1.0;
    }

    printf("\n");
    printf("Multiplying graph Laplacian by constant vector \n");
    printf("    using JIT-compiled block matvec.\n");
    blockSparseMatrixVectorMultiply(A, x, y);

    for (i = 0; i < nn; i++) {
        if (y[i] != 0.0) {
            printf("%d %g\n", i, y[i]);
            return 1;
        }
    }

    printf("JIT-compiled block matvec gave correct result of 0! Wahoo!\n");


    /* Free up the memory used */
    destroyGraph(g);
    free(g);

    destroySparseMatrix(B);
    free(B);

    destroyBlockSparseMatrix(A);
    free(A);

    return 0;
}
