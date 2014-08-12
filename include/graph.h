#include "stack.h"

#ifndef GRAPH_H
#define GRAPH_H

typedef struct {
    int m, n, ne;
    Stack *lists;
} Graph;


void initGraph(Graph *g, int m, int n);
void destroyGraph(Graph *g);

int connected(Graph *g, int i, int j);
int getDegree(Graph *g, int i);
int getMaxDegree(Graph *g);
void getNeighbors(Graph *g, int *neighbors, int i);

void addEdge(Graph *g, int i, int j);
void deleteEdge(Graph *g, int i, int j);

#endif
