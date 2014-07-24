#include "sparse_matrix.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>



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


    /* Build a matrix on the graph `g` */
    SparseMatrix *A;
    A = malloc( sizeof(SparseMatrix) );
    initSparseMatrix(A, nn, nn, g);


    /* Construct the Laplacian of the graph `g` */
    int d = getMaxDegree(g);
    int *neighbors;
    neighbors = (int *) malloc( d * sizeof(int) );

    for (i = 0; i < nn; i++) {

        d = getDegree(g, i);
        getNeighbors(g, neighbors, i);

        for (k = 0; k < d; k++) {
            j = neighbors[k];

            addValue(A, i, i, +1.0);
            addValue(A, i, j, -1.0);
        }
    }

    free(neighbors);


    /* Make some vectors and multiply them by `A` */
    double *x, *y;
    x = (double *) malloc( nn * sizeof(double) );
    y = (double *) malloc( nn * sizeof(double) );
    for (i = 0; i < nn; i++) {
        x[i] = 1.0;
        y[i] = 1.0;
    }

    sparseMatrixVectorMultiply(A, x, y);

    for (i = 0; i < nn; i++) {
        if (y[i] != 0.0) printf("%d %g\n", i, y[i]);
    }


    /* Free up the memory used */
    destroyGraph(g);
    free(g);

    destroySparseMatrix(A);
    free(A);

    return 0;
}
