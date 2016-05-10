#ifndef __MAXSAT_H__
#define __MAXSAT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define max(A,B) ((A) >= (B)?(A):(B))
#define min(A,B) ((A) <= (B)?(A):(B))

/* Data structure meant to store the global result */
typedef struct output {
    int max;
    int nMax;
    int *path;
} output;

/* Data structure with all the variables needed on the recursive algorithm */
typedef struct node {
    int level, Mc, mc;
    struct node *u, *l, *r;
    int *vars, *cls_evals;
} node;

node *create_node(int Mc, int mc, int level, int ncl, node * father);
void delete_node(node *ptr);
void set_path(output *op, node *ptr, int len);

#endif

