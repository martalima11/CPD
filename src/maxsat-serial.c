#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define max(A,B) ((A) >= (B)?(A):(B))
#define min(A,B) ((A) <= (B)?(A):(B))
#define DEBUG 0

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

/* Recursive function used to generate the intended results */
void solve(node *ptr, int nvar, int **cls, int ncl, output *op){
    int i, j, res;

    for(i = 0; i < ncl; i++){
        if(ptr->level){
            /* Initializes the position based on father node */
            ptr->cls_evals[i] = ptr->u->cls_evals[i];

            /* The clause only needs to be evaluated
            in case there is still no veredict about it */
            if(!ptr->u->cls_evals[i]){

                /* The clauses' variables are comparared
                 up to the present level */
                for(j = 0; cls[i][j] && abs(cls[i][j]) <= ptr->level; j++){

                    /* The result of the variable atribuition
                    is calculated, for the current clause */
                    res = cls[i][j] + ptr->vars[abs(cls[i][j])-1];

                    /* If the result is 0, then the values are symmetric so
                    there can be no conclusion (about this variable).
                    Otherwise (res != 0), the clause can be evaluated as true
                    and will no longer be evaluated. */
                    if(res){
                        ptr->cls_evals[i] = 1;
                        ptr->mc++;
                        break;
                    }
                }
                /* If the clause has no more variables to be evaluated and
                it still has no solution, then the clause is evaluated as false */
                if(!cls[i][j] && !ptr->cls_evals[i]){
                    ptr->cls_evals[i] = -1;
                    ptr->Mc--;
                }
            }
        }else{
            /* The first node is initialized */
            ptr->cls_evals[i] = 0;
        }
    }

    /* After calculation on the current node */
    /* For debug purposes, it's possible to know the
    status for the current node */
    if(DEBUG){
        for(j=0; j<ptr->level; j++){
            printf("     ");
        }
        printf("n: %d; Mc: %d; mc: %d\n", ptr->level, ptr->Mc, ptr->mc);
    }

    /* Check if the best possible outcome was reached
    If TRUE then there is no need to proceed further down the tree (pruning)
    Else, create children nodes and corresponding information for both */
    if(ptr->Mc == ptr->mc){
        if(ptr->Mc == op->max){
            op->nMax += pow(2, (nvar - ptr->level));
        }else if(ptr->Mc > op->max){
            op->max = ptr->Mc;
            op->nMax = pow(2, (nvar - ptr->level));
            set_path(op, ptr, nvar);
        }
    }else if(ptr->level < nvar && op->max <= ptr->Mc){
        ptr->l = create_node(ptr->Mc, ptr->mc, ptr->level+1, ncl, ptr);
        ptr->r = create_node(ptr->Mc, ptr->mc, ptr->level+1, ncl, ptr);

        for(i = 0; i < ptr->level; i++){
            ptr->l->vars[i] = ptr->vars[i];
            ptr->r->vars[i] = ptr->vars[i];
        }

        ptr->l->vars[ptr->level] = -(ptr->level + 1);
        ptr->r->vars[ptr->level] =  (ptr->level + 1);

        solve(ptr->l, nvar, cls, ncl, op);
        solve(ptr->r, nvar, cls, ncl, op);

        delete_node(ptr->l);
        delete_node(ptr->r);
    }
    return;
}

/* Main function */
int main(int argc, char *argv[]){
    char * ext;
    char * out_file;

    FILE * f_in = NULL;
    FILE * f_out = NULL;

    char buf[128];
    char *p;
    int i, n;
    int nvar, ncl;

    int **cls;
    node *btree;
    output *op;

    double start, end;

    if(argc != 2) exit(1);

    start = omp_get_wtime();

    /*abertura de ficheitos*/
    ext = strrchr(argv[1], '.');
    if(!ext || strcmp(ext, ".in")) exit(1);
    f_in = fopen(argv[1], "r");
    if(!f_in) exit(1);
    out_file = (char*) malloc((strlen(argv[1])+2)*sizeof(char));
    strcpy(ext, ".out");
    strcpy(out_file, argv[1]);
    f_out = fopen(out_file, "w");
    free(out_file);
    if(!f_out){
        fclose(f_in);
        exit(1);
    }
    /* #variaveis, #clausulas */
    if(fgets(buf, 128, f_in)){
        if(DEBUG)
            printf("%s", buf);
        if((sscanf(buf, "%d %d", &nvar, &ncl)) != 2){
            fclose(f_out);
            fclose(f_in);
            exit(1);
        }
    }else{
        fclose(f_out);
        fclose(f_in);
        exit(1);
    }

    cls = (int**) malloc(ncl*sizeof(int*));

    for(i = 0; i < ncl; i++){
        cls[i] = (int*) malloc((min(nvar, 20) + 1) * sizeof(int));
        n = 0;
        if(fgets(buf, 128, f_in)){
            p = strtok(buf, " \n");
            while(p){
                if(DEBUG)
                    printf("%2d ", atoi(p));
                cls[i][n++] = atoi(p);
                p = strtok(NULL, " \n");
            }
            if(DEBUG)
                printf("\n");
        }else{
            fclose(f_out);
            fclose(f_in);
            exit(1);
        }
    }

    btree = create_node(ncl, 0, 0, ncl, NULL);

    op = (output*) malloc(sizeof(output));
    op->path = (int*) malloc(nvar * sizeof(int));
    op->max = -1;
    op->nMax = 0;

    solve(btree, nvar, cls, ncl, op);

    fprintf(f_out, "%d %d\n", op->max, op->nMax);
    if(DEBUG)
        printf("Max: %d; nMax: %d\n", op->max, op->nMax);

    for(i=0; i<nvar; i++){
        fprintf(f_out, "%d ", op->path[i]);
        if(DEBUG)
            printf("%d ", op->path[i]);
    }
    if(DEBUG)
        printf("\n");

    free(op->path);
    free(op);
    delete_node(btree);

    for(i=0; i<ncl; i++){
        free(cls[i]);
    }

    free(cls);
    fclose(f_out);
    fclose(f_in);

    end = omp_get_wtime();
    printf("Elapsed time: %.09f\n", end-start);

    return 0;
}
