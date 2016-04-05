#include "maxsat.h"

/* Function used to generate new (node *) instances */
node *create_node(int Mc, int mc, int level, int ncl, node *father){
    node *new_node = (node*) malloc(sizeof(node));
    new_node -> level = level;
    new_node -> Mc = Mc;
    new_node -> mc = mc;
    new_node -> u = father;
    new_node -> l = NULL;
    new_node -> r = NULL;
    new_node -> vars = (int*) malloc(level * sizeof(int));
    new_node -> cls_evals = (int*) malloc(ncl * sizeof(int));
    return new_node;
}

/* Function used for memory clean-up */
void delete_node(node *ptr){
    free(ptr->vars);
    free(ptr->cls_evals);
    free(ptr);
    return;
}

/* Function used to generate possible results */
void set_path(output *op, node *ptr, int len){
    int i;
    for(i=0; i<len; i++){
        if(i < ptr->level)
            op->path[i] = ptr->vars[i];
        else
            op->path[i] = i + 1;
    }
    return;
}
