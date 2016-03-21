#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define max(A,B) ((A) >= (B)?(A):(B))
#define DEBUG 1

typedef struct node {
    int level, Mc, mc;
    struct node *u, *l, *r;
    int *vars, *cls_evals;
} node;

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

void delete_node(node *ptr){
    free(ptr->vars);
    free(ptr->cls_evals);
    free(ptr);
    return;
}

void solve(node *ptr, int nvar, int **cls, int ncl){
    int i, j, res;

    for(i = 0; i < ncl; i++){
        if(ptr->level){
            /* Inicializa a posicao com base no nó progenitor */
            ptr->cls_evals[i] = ptr->u->cls_evals[i];
            /* So precisa de calcular se a clausula nao for ainda verdadeira nem falsa */
            if(!ptr->u->cls_evals[i]){
                /* Compara variaveis das clausulas ate ao presente nivel */
                for(j=0; cls[i][j] && abs(cls[i][j]) <= ptr->level; j++){
                    /* calcula o resultado, da atribuicao de variavel, na clausula */
                    res = cls[i][j] + ptr->vars[abs(cls[i][j])-1];
                    /* se resultado for 0 quer dizer que os valores sao simetricos,
                      caso contrario a clausula e verdadeira e nao vale a pena ver as outras variaveis*/
                    if(res){
                        ptr->cls_evals[i] = 1;
                        ptr->mc++;
                        break;
                    }
                }
                /* Se a clausula tiver sido lida ate ao fim e ainda assim nao for verdadeira,
                   significa que ela e falsa */
                if(!cls[i][j] && !ptr->cls_evals[i]){
                    ptr->cls_evals[i] = -1;
                    ptr->Mc--;
                }
            }
        }else{
            /* Inicializa o promeiro no */
            ptr->cls_evals[i] = 0;
        }
    }

    /* fim de calculos */

    for(j=0; j<ptr->level; j++){
        printf("     ");
    }
    printf("n: %d; Mc: %d; mc: %d\n", ptr->level, ptr->Mc, ptr->mc);

    /*Ciclo para ver o valor das clausulas em cada iteraceo */
    /*
    printf("\n");
    for(i = 0; i < ncl; i++){
        printf("Clause #%d: %d\n", i+1, ptr->cls_evals[i]);
    }
    printf("\n");
    */
    /* fecundar duas posições de memoria e executar processo de adocao nos dois */
    if(ptr->Mc == ptr->mc){
        /*printf("DONE\n");*/
    }else if(ptr->level < nvar){
        ptr->l = create_node(ptr->Mc, ptr->mc, ptr->level+1, ncl, ptr);
        ptr->r = create_node(ptr->Mc, ptr->mc, ptr->level+1, ncl, ptr);
        for(i=0;i<ptr->level;i++){
            ptr->l->vars[i] = ptr->vars[i];
            ptr->r->vars[i] = ptr->vars[i];
        }
        ptr->l->vars[ptr->level] = -(ptr->level+1);
        ptr->r->vars[ptr->level] = ptr->level+1;
        solve(ptr->l, nvar, cls, ncl);
        solve(ptr->r, nvar, cls, ncl);
        delete_node(ptr->l);
        delete_node(ptr->r);
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

    btree = create_node(ncl, 0, 0, ncl, NULL);

    solve(btree, nvar, cls, ncl);

    delete_node(btree);

    for(i=0; i<ncl; i++){
        free(cls[i]);
    }

    free(cls);
    fclose(f_out);
    fclose(f_in);

    return 0;
}
