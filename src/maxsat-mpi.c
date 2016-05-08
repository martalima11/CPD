#include <omp.h>
#include <mpi.h>

#include "maxsat.h"

#define DEBUG 0

void master(int ncls, int nvar){
	int proc, i, init_level, task_size;
	int * task;
	int * proc_queue;
	int stop = 0;

	MPI_Comm_size(MPI_COMM_WORLD, &proc);

	/* Processor Queue. 0 - Idle ; 1 - Busy*/
	proc_queue = (int *) malloc(proc * sizeof(int));
	memset(proc_queue, 0, sizeof(proc_queue));

	/* Task Pool */
	/* A task is the concatenation of 4 integers and a vector
	 * compromising the "path taken to a node"
	 * (Mc, mc, level, vars) needed to create a node and
	 * start the working process. */

	 init_level = min(log2(proc), nvars);
	 task_size = nvar + 3;
	 task = (int *) malloc(task_size * sizeof(int));
	 task[0] = ncls;
	 task[1] = 0;
	 task[2] = init_level;

	 /* Initiate Tasks */
	 for(i = 0; i < pow(2, init_level); i++){
		for(j = 3; j < min(min(nvar, 20) + 1, init_level) + 3; j++){    /* criar variavel para condição de paragaem*/
			if((i/pow(2, (j-3))) % 2){
				task[j] = j - 2;
			}else{
				task[j] = 2 - j;
			}
		}
		if( i < proc - 1 ){
			MPI_Send((void *) task, task_size, MPI_INT, i+1, TASK_TAG, MPI_COMM_WORLD);
		} else {
			insert_task(tpool, task);
		}
	}

	while(!stop){								/* especificar o tag para ser fixe*/
		MPI_RECV(task, task_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	}
	/* Memory Clean-Up */
	free(proc_queue);
}

void slave(){

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
    int nvar, ncl, offset;

    int **cls;
    node *btree;
    output *op;

    int id, data_size[2];

    // double start, end;
	double elapsed_time;

    if(argc != 2) exit(1);

	/* MPI configuration */
	MPI_Init(&argc, &argv);
	MPI_Barrier(MPI_COMM_WORLD);

	elapsed_time = -MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    if(!id){
		/* IO configuration */
		ext = strrchr(argv[1], '.');
		if(!ext || strcmp(ext, ".in")){
			MPI_Finalize();
			exit(1);
		}

		f_in = fopen(argv[1], "r");
		if(!f_in){
			MPI_Finalize();
			exit(1);
		}

		out_file = (char*) malloc((strlen(argv[1]) + 2) * sizeof(char));
		strcpy(ext, ".out");
		strcpy(out_file, argv[1]);
		f_out = fopen(out_file, "w");
		free(out_file);
		if(!f_out){
			fclose(f_in);
			MPI_Finalize();
			exit(1);
		}

		/* Root node gets #variables and #clauses */
		if(fgets(buf, 128, f_in)){
			if(DEBUG)
				printf("%s", buf);
			if((sscanf(buf, "%d %d", &nvar, &ncl)) != 2){
				fclose(f_out);
				fclose(f_in);
				MPI_Finalize();
				exit(1);
			}
			data_size[0] = nvar;
			data_size[1] = ncl;
		}else{
			fclose(f_out);
			fclose(f_in);
			MPI_Finalize();
			exit(1);
		}

	}

	/* Node 0 sends #variables and #clauses to all processors */
	MPI_Bcast(data_size, 2, MPI_INT, 0, MPI_COMM_WORLD);
	if(id){
		nvar = data_size[0];
		ncl = data_size[1];
	}

    /* Data structure initialization for all processors */
    /* Vector with pointers to vector with all clauses */
    cls = (int**) malloc(ncl*sizeof(int*));
    /* Vector with all clauses and variables (Nvars * nCls) */
    offset = (min(nvar, 20) + 1);

    cls[0] = (int*) malloc(ncl * offset * sizeof(int));
    for(i = 1; i < ncl; i++)
		cls[i] = cls[i - 1] + offset;

	/* Node 0 will initialize data structure*/
    if(!id){
		for(i = 0; i < ncl; i++){
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
				MPI_Finalize();
				exit(1);
			}
		}
	}

	/* Node 0 Sends data structure for all processors */
	MPI_Bcast(cls[0], ncl * offset, MPI_INT, 0, MPI_COMM_WORLD);

    //btree = create_node(ncl, 0, 0, ncl, NULL);



    /* Main algorithm */

     if(!id){
		// Master

		/* output structure*/
		op = (output*) malloc(sizeof(output));
		op->path = (int*) malloc(nvar * sizeof(int));
		op->max = -1;
		op->nMax = 0;

		master();

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

		/* Memory clean-up */
		free(op->path);
		free(op);

		fclose(f_out);
		fclose(f_in);

	 }else{
		// Slaves
		slaves();
	}

    delete_node(btree);

    free(cls[0]);
    free(cls);

    elapsed_time += MPI_Wtime();
    if(!id)
		printf("Elapsed time: %.09f\n", elapsed_time);

    MPI_Finalize();

    return 0;
}
