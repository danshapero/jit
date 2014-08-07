
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#include "jit_block_sparse_matrix_kernels.h"



int main(int arg, char **argv) {

    /* -------------------------------------------------------------------
      /  Build a random graph                                            /
     -------------------------------------------------------------------- */
    int i, j, k, nn, ne;
    double q, p;
    nn = 512;


    Graph *g;
    g = (Graph *) malloc(sizeof(Graph));
    initGraph(g, nn, nn);

    srand(time(NULL));
    p = log(1.0 * nn) / nn;
    for (i = 0; i < nn; i++) {
        addEdge(g, i, i);

        for (j = i + 1; j < nn; j++) {
            q = (1.0 * rand()) / RAND_MAX;
            if (q < p) {
                addEdge(g, i, j);
                addEdge(g, j, i);
            }
        }
    }

    printf("Done building Erdos-Renyi graph.\n");
    printf("    Vertices:   %d\n", nn);
    printf("    Edges:      %d\n", g->ne);
    printf("    Max degree: %d\n", getMaxDegree(g));


    /* -------------------------------------------------------------------
      /  Build the graph Laplacian                                       /
     -------------------------------------------------------------------- */
    SparseMatrix *B;
    B = malloc( sizeof(SparseMatrix) );
    initSparseMatrix(B, nn, nn, g);

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


    /* -------------------------------------------------------------------
      /  Blockify the matrix                                             /
     -------------------------------------------------------------------- */
    BlockSparseMatrix *A;
    A = malloc( sizeof(BlockSparseMatrix) );
    blockify(A, B, 8, 8);

    printf("Blockifying sparse matrix.\n");


    /* Make some vectors and multiply them by `A` */
    double *x, *y, *z, error;
    x = (double *) malloc( nn * sizeof(double) );
    y = (double *) malloc( nn * sizeof(double) );
    z = (double *) malloc( nn * sizeof(double) );

    for (i = 0; i < nn; i++) {
        x[i] = (1.0 * rand()) / RAND_MAX;
        y[i] = 1.0;
        z[i] = 0.0;
    }

    sparseMatrixVectorMultiply(B, x, z);


    printf("Multiplying blockified graph Laplacian by random vector.\n");
    blockSparseMatrixVectorMultiply(A, x, y);

    error = 0.0;
    for (i = 0; i < nn; i++) {
        q = y[i] - z[i];
        error = error + q * q;
    }
    error = sqrt(error) / nn;

    printf("RMS difference between regular and block matrix-vector\n");
    printf("    multiplication of a random vector: %g\n", error);

    printf("\n");


    /* -------------------------------------------------------------------
      /  JIT compile block matrix-vector multiplication                  /
     -------------------------------------------------------------------- */
    TCCState *CompilerState;
    CompilerState = tcc_new();


    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("JIT compiling the block matrix-vector multiplication kernel:\n");
    BlockMatvec mv;
    mv = jitCompileBlockMatvec(CompilerState, 8, 8);
    printf("Done JIT compiling matvec kernel!\n");

    printf("Location in memory of native block matvec:  %x\n",
                                            (int)(&native_bcsr_matvec));
    printf("Location in memory of JIT block matvec:     %x\n", (int)mv);


    printf("Changing A's implementation of block matvec to point to \n");
    printf("    the JIT-compiled matvec.\n");
    A->matvec = mv;
    printf("New location of A's block matvec implementation: %x\n",
                                                        (int)A->matvec);

    for (i = 0; i < nn; i++) {
        y[i] = 1.0;
    }

    printf("\n");
    printf("Multiplying graph Laplacian by random vector \n");
    printf("    using JIT-compiled block matvec.\n");
    blockSparseMatrixVectorMultiply(A, x, y);

    error = 0.0;
    for (i = 0; i < nn; i++) {
        q = y[i] - z[i];
        error = error + q * q;
    }
    error = sqrt(error) / nn;

    printf("RMS difference between regular and JIT block matrix-vector\n");
    printf("    multiplication of a random vector: %g\n", error);

    printf("\n");



    /* -------------------------------------------------------------------
      /  JIT compile block specialized matrix-vector multiplication      /
     -------------------------------------------------------------------- */

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("JIT compiling a specialized block matrix-vector \n");
    printf("    kernel for the block size (%d, %d).\n", 8, 8);
    BlockMatvec mva;
    mva = jitCompileSpecializedBlockMatvec(CompilerState, 8, 8);
    printf("Location in memory of old JIT block matvec: %x\n", (int)mv);
    printf("Location in memory of new JIT block matvec: %x\n", (int)mva);
    printf("Changing A's implementation of block matvec to point to \n");
    printf("    the new JIT-compiled matvec.\n");
    A->matvec = mva;


    for (i = 0; i < nn; i++) {
        y[i] = 1.0;
    }

    printf("Multiplying graph Laplacian by random vector \n");
    printf("    using JIT-compiled block matvec.\n");
    blockSparseMatrixVectorMultiply(A, x, y);

    error = 0.0;
    for (i = 0; i < nn; i++) {
        q = y[i] - z[i];
        error = error + q * q;
    }
    error = sqrt(error) / nn;

    printf("RMS difference between regular and JIT block matrix-vector\n");
    printf("    multiplication of a random vector: %g\n", error);

    printf("\n");



    /* -------------------------------------------------------------------
      /  JIT compile unrolled block matrix-vector multiplication         /
     -------------------------------------------------------------------- */

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("JIT compiling an unrolled block matrix-vector \n");
    printf("    kernel for the block size (%d, %d).\n", 8, 8);
    BlockMatvec mvu;
    mvu = jitCompileUnrolledBlockMatvec(CompilerState, 8, 8);
    printf("Location in memory of unrolled JIT block matvec: %x\n", (int)mvu);
    printf("Changing A's implementation of block matvec to point to \n");
    printf("    the new JIT-compiled unrolled matvec.\n");
    A->matvec = mvu;


    for (i = 0; i < nn; i++) {
        y[i] = 1.0;
    }

    printf("Multiplying graph Laplacian by random vector \n");
    printf("    using JIT-compiled block matvec.\n");
    blockSparseMatrixVectorMultiply(A, x, y);

    error = 0.0;
    for (i = 0; i < nn; i++) {
        q = y[i] - z[i];
        error = error + q * q;
    }
    error = sqrt(error) / nn;

    printf("RMS difference between regular and JIT block matrix-vector\n");
    printf("    multiplication of a random vector: %g\n", error);

    printf("\n");



    /* Free up the memory used */
    destroyGraph(g);
    free(g);

    destroySparseMatrix(B);
    free(B);

    destroyBlockSparseMatrix(A);
    free(A);

    free(x);
    free(y);
    free(z);

    tcc_delete(CompilerState);

    return 0;
}
