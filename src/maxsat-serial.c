#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

int main(int argc, char *argv[]){

    char * ext;
    char * out_file;

    FILE * f_in = NULL;
    FILE * f_out = NULL;

    char buf[128];
    char *p;
    int nvar, ncl;
    int i;

    if(argc != 2) exit(1);

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

    for(i=0;i<ncl;i++){
        if(fgets(buf, 128, f_in)){
            p = strtok(buf, " \n");
            while(p){
                printf("%d ", atoi(p));
                p = strtok(NULL, " \n");
            }
            printf("\n");
        }else{
            fclose(f_out);
            fclose(f_in);
            exit(1);
        }
    }

    fclose(f_out);
    fclose(f_in);

    return 0;
}
