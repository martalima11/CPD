#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define max(A,B) ((A) >= (B)?(A):(B))

typedef struct node {
    int level, Mc, mc;
    struct node *u, *l, *r;
    int *vars, *cls_evals;
} node;

node *create_node(int Mc, int mc, int level, node *father){

    node *new_node = (node*) malloc(sizeof(node));
    new_node -> level = level;
    new_node -> Mc = Mc;
    new_node -> mc = mc;
    new_node -> u = father;
    new_node -> l = NULL;
    new_node -> r = NULL;
    /*new_node -> vars = (int*) malloc(level * sizeof(int));*/
    /*new_node -> cls_evals = (int*) malloc( sizeof(int));*/
    /* popular com os valores do nivel anterior? */
    return new_node;
}

void delete_node(node *ptr){
    /*free(ptr->cls_evals);
    free(ptr->vars);*/
    free(ptr);
    return;
}

void solve(node *ptr, int i, int nvar){
    int j;
    for(j=0; j<ptr->level; j++){
        printf("     ");
    }
    printf("n: %d; i: %d\n", ptr->level, i);
    /*fazer calculos e alterar Mc e mc*/

    /* fim de calculos, fecundar duas posições de memoria e executar processo de adocao nos dois */
    if(ptr->level < nvar){
        ptr->l = create_node(4, 0, ptr->level+1, ptr);
        ptr->r = create_node(4, 0, ptr->level+1, ptr);
        solve(ptr->l, 2*i, nvar);
        solve(ptr->r, 2*i+1, nvar);
        free(ptr->l);
        free(ptr->r);
    }
    return;
}

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

    if(argc != 2) exit(1);

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

    for(i=0; i<ncl; i++){
        cls[i] = (int*) malloc((max(nvar, 20) + 1) * sizeof(int));
        n = 0;
        if(fgets(buf, 128, f_in)){
            p = strtok(buf, " \n");
            while(p){
                printf("%2d ", atoi(p));
                cls[i][n++] = atoi(p);
                p = strtok(NULL, " \n");
            }
            printf("\n");
        }else{
            fclose(f_out);
            fclose(f_in);
            exit(1);
        }
    }

    btree = create_node(ncl, 0, 0, NULL);
    printf("Node %d has Mc: %d and mc: %d\n", btree->level, btree->Mc, btree->mc);
    solve(btree, 0, nvar);
    free(btree);

    for(i=0; i<ncl; i++){
        free(cls[i]);
    }

    free(cls);
    fclose(f_out);
    fclose(f_in);

    return 0;
}
