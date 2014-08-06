#include "graph.h"
#include <stdlib.h>


void initGraph(Graph *g, int m, int n) {
    g->m = m;
    g->n = n;
    g->ne = 0;

    g->lists = (Stack*) malloc( m * sizeof(Stack) );

    for (int i = 0; i < m; i++) {
        initStack(g->lists + i);
    }
}


void destroyGraph(Graph *g) {
    for (int i = 0; i < g->m; i++) {
        destroyStack(g->lists + i);
    }

    free(g->lists);

    g->m = 0;
    g->n = 0;
    g->ne = 0;
}


int connected(Graph *g, int i, int j) {
    for (int k = 0; k < g->lists[i].len; k++) {
        if (getEntry(g->lists + i, k) == j) return 1;
    }

    return 0;
}


int getDegree(Graph *g, int i) {
    return g->lists[i].len;
}


int getMaxDegree(Graph *g) {
    int d = 0;

    for (int i = 0; i < g->m; i++) {
        d = (d > g->lists[i].len) ? d : g->lists[i].len;
    }

    return d;
}


void getNeighbors(Graph *g, int *neighbors, int i) {
    int j;
    int d;
    d = g->lists[i].len;
    for (int k = 0; k < d; k++) {
        j = getEntry(g->lists + i, k);
        neighbors[k] = j;
    }
}


void addEdge(Graph *g, int i, int j) {
    if (!connected(g, i, j)) {
        push(g->lists + i, j);
        g->ne++;
    }
}


void deleteEdge(Graph *g, int i, int j) {
    int k, index, jt;
    index = -1;
    for (k = 0; k < g->lists[i].len; k++) {
        if (getEntry(g->lists + i, k) == j) index = k;
    }

    if (index != -1) {
        jt = pop(g->lists + i);
        if (jt != j) setEntry(g->lists + i, index, jt);
        g->ne--;
    }
}



