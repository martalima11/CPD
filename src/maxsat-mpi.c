#include <omp.h>
#include <mpi.h>

#include "maxsat.h"
#include "task.h"

#define DEBUG 0
#define TASK_TAG 0
#define STOP_TAG 1

int check_empty(int *proc_queue, int queue_size){
	int i;
	for(i = 0; i < queue_size && !proc_queue[i]; i++);
	if(i == queue_size){
		return 1;
	}else{
		return 0;
	}
}

int get_proc(int *proc_queue, int queue_size){
	int i;
	for(i = 0; i < queue_size && proc_queue[i]; i++);
	if(i == queue_size){
		return -1;
	}else{
		return i;
	}
}

/* Change Structure of task for sending the result to master*/
void updateTask(int * task, output * op, int nvar){
	int i;
	task[TASK_max] = op->max;
	task[TASK_nmax] = op->nMax;
	for(i = 0; i < nvar; i++)
		task[TASK_maxpath + i] = op->path[i];
	return;
}

void updateMax(output * op, int * buffer, int path_size){
	int i;
	if(buffer[TASK_max] == op->max){
		op->nMax += buffer[TASK_nmax];
	}else if(buffer[TASK_max] > op->max){
		op->max = buffer[TASK_max];
		op->nMax = buffer[TASK_nmax];
		for(i = 0; i < path_size; i++){
			 op->path[i] = buffer[TASK_maxpath + i];
		}
	}
}

/* Recursive function used to generate the intended results */
void solve(node *ptr, int nvar, int **cls, int ncl, output * op, int first){
    int i, j, res;
	int *task;
	int task_size = nvar +3;

	for(i = 0; i < ncl; i++){
		/* Initializes the position based on father node */
		if(!first)
			ptr->cls_evals[i] = ptr->u->cls_evals[i];

		/* The clause only needs to be evaluated
		in case there is still no veredict about it */
		if(!ptr->cls_evals[i]){

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
	}



    /* After calculation on the current node */
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
        if(!first) ptr->l = create_node(ptr->Mc, ptr->mc, ptr->level+1, ncl, ptr);
        else{
			task = (int *) malloc(task_size * sizeof(int));
			task[TASK_Mc] = ptr->Mc;
			task[TASK_mc] = ptr->mc;
			task[TASK_level] = ptr->level + 1;
		}
        ptr->r = create_node(ptr->Mc, ptr->mc, ptr->level+1, ncl, ptr);

        for(i = 0; i < ptr->level; i++){
            if(!first) ptr->l->vars[i] = ptr->vars[i];
            else task[TASK_vars+i] = ptr->vars[i];

			ptr->r->vars[i] = ptr->vars[i];
        }

		if(!first){
			ptr->l->vars[ptr->level] = -(ptr->level + 1);
			solve(ptr->l, nvar, cls, ncl, op, 0);
			delete_node(ptr->l);
		}else{
			task[TASK_vars + ptr->level] = -(ptr->level + 1);
			if(DEBUG)
				printf("Saving first born from evil twin\n");
			MPI_Send((void *) task, task_size, MPI_INT, 0, TASK_TAG, MPI_COMM_WORLD);
			free(task);
		}
        ptr->r->vars[ptr->level] =  (ptr->level + 1);
        solve(ptr->r, nvar, cls, ncl, op, 0);
        delete_node(ptr->r);

    }
    return;
}

void master(int ncl, int nvar, int ** cls, output * op){
	int nproc, init_level, task_size, path_size;
	int i, j, p;
	int * buffer;
	int * proc_queue;
	int stop = 0;

	task_pool tpool = NULL;

	MPI_Status status;

	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	if(nproc > 1){
		/* Processor Queue. 0 - Idle ; 1 - Busy*/
		proc_queue = (int *) malloc((nproc - 1) * sizeof(int));
		memset(proc_queue, 0, (nproc - 1) * sizeof(int));

		/* Task Pool */
		/* A task is the concatenation of 4 integers and a vector
		 * compromising the "path taken to a node"
		 * (Mc, mc, level, vars) needed to create a node and
		 * start the working process. */
		 init_level = min(log2(nproc), nvar);
		 task_size = nvar + 3;
		 buffer = (int *) malloc(task_size * sizeof(int));
		 buffer[TASK_Mc] = ncl;
		 buffer[TASK_mc] = 0;
		 buffer[TASK_level] = init_level;

		 /* Initiate Tasks */
		 path_size = min(min(nvar, 20) + 1, init_level) + TASK_vars;

		 for(i = 0; i < pow(2, init_level); i++){
			for(j = TASK_vars; j < path_size; j++){
				if((int)(i/pow(2, (j - 3))) % 2){
					buffer[j] = j - 2;
				}else{
					buffer[j] = 2 - j;
				}
			}
			if( i < nproc - 1 ){
				/* começa a enviar para o processador 1, pois o 0 é o main */
				if(DEBUG)
					printf("Sending 'TASK' to process #%d from ROOT\n", i + 1);
				MPI_Send((void *) buffer, task_size, MPI_INT, i + 1, TASK_TAG, MPI_COMM_WORLD);
				proc_queue[i] = 1;
			}else{
				insert_task(&tpool, buffer, task_size);
			}
		}

		while(!stop){
			if(DEBUG)
				printf("ROOT Receiving\n");
			MPI_Recv(buffer, task_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

			switch(status.MPI_TAG){
				case TASK_TAG:
					p = get_proc(proc_queue, nproc - 1);
					if(p == -1){
						insert_task(&tpool, buffer, task_size);
					}else{
						// check which is the task that it should send
						if(DEBUG)
							printf("ROOT sends work to process #%d\n", p + 1);
						MPI_Send((void *) buffer, task_size, MPI_INT, p + 1, TASK_TAG, MPI_COMM_WORLD);
						proc_queue[p] = 1;
					}
					break;
				case STOP_TAG:
					if(DEBUG)
						printf("ROOT working on received task from #%d\n", status.MPI_SOURCE);
					updateMax(op, buffer, nvar);

					/* Somewhere around here the master should evaluate the task queue
					 * and discard tasks with possible maximums (Mc) lower than
					 * op->max */

					switch(get_task(&tpool, buffer, task_size, op->max)){
						case(-1):
							/* processador 1 indexado na posição 0, pois o main não conta para o vector */
							proc_queue[status.MPI_SOURCE - 1] = 0;
							break;
						case(0):
							if(DEBUG)
								printf("ROOT sends work to process #%d\n", status.MPI_SOURCE);
							MPI_Send((void *) buffer, task_size, MPI_INT, status.MPI_SOURCE, TASK_TAG, MPI_COMM_WORLD);
							break;
						default:
							/* Just in case */
							break;
						}
					break;
				default:
					/* Just in case */
					break;
			}
			if(check_empty(proc_queue, nproc - 1)){
				stop = 1;
			}
		}
		for(i = 0; i < nproc - 1; i++){
			if(DEBUG)
				printf("Sending 'STOP' to process #%d from ROOT\n", i+1);
			MPI_Send((void *) buffer, task_size, MPI_INT, i + 1, STOP_TAG, MPI_COMM_WORLD);
		}

		/* Memory Clean-Up */
		free(proc_queue);
		free(buffer);

	}else{	
		// There is only one processor
		node * btree;
		
		/* Initialize subtrees */
		btree = create_node(ncl, 0, 0, ncl, NULL);
		for(i = 0; i < btree->level; i++)
            btree->vars[i] = 0;
		for(i = 0; i < ncl; i++)
			btree->cls_evals[i] = 0;


		btree->l = create_node(ncl, 0, 1, ncl, btree);
        btree->r = create_node(ncl, 0, 1, ncl, btree);

        btree->l->vars[0] = -1;
        btree->r->vars[0] =  1;

        solve(btree->l, nvar, cls, ncl, op, 0);
        solve(btree->r, nvar, cls, ncl, op, 0);
			
        delete_node(btree->l);
        delete_node(btree->r);
        delete_node(btree);
		
	}
	return;
}

void slave(int id, int ncl, int nvar, int ** cls, output * op){
	int * task;
	int i, task_size;
	node *btree;

	MPI_Status status;

	/* Allocate task */
	task_size = nvar + 3;

	task = (int *) malloc(task_size * sizeof(int));

	while(1){
		/* Receive task to work on */
		if(DEBUG)
			printf("Process #%d Receiving\n", id);
		MPI_Recv(task, task_size, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		/* Clean exit */
		if(status.MPI_TAG == STOP_TAG){
			free(task);
			break;
		}

		op->max = task[TASK_mc];
		op->nMax = 0;

		/* Initialize node based on task */
		btree = create_node(ncl, 0, task[TASK_level], ncl, NULL);
		for(i = 0; i < btree->level; i++)
            btree->vars[i] = task[TASK_vars + i];
		for(i = 0; i < ncl; i++)
			btree->cls_evals[i] = 0;

		/* Work on subtree */
		solve(btree, nvar, cls, ncl, op, 1);

		/* Send result */
		/* update task with op's information */
		updateTask(task, op, nvar);
		if(DEBUG)
			printf("Sending 'STOP' to ROOT from process #%d\n", id);
		MPI_Send((void *) task, task_size, MPI_INT, 0, STOP_TAG, MPI_COMM_WORLD);
		delete_node(btree);
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
    int nvar, ncl, offset;

    int **cls;
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

	/* output structure*/
	op = (output*) malloc(sizeof(output));
	op->path = (int*) malloc(nvar * sizeof(int));
	op->max = -1;
	op->nMax = 0;

    /* Main algorithm */
    if(!id){
		// Master
		master(ncl, nvar, cls, op);

		fprintf(f_out, "%d %d\n", op->max, op->nMax);
		if(DEBUG)
			printf("Max: %d; nMax: %d\n", op->max, op->nMax);

		for(i=0; i<nvar; i++){
			fprintf(f_out, "%d ", op->path[i]);
			if(DEBUG)
				printf("%d ", op->path[i]);
		}
		if(DEBUG) printf("\n");

		fclose(f_out);
		fclose(f_in);

	}else{
		// Slaves
		slave(id, ncl, nvar, cls, op);
	}

	/* Memory clean-up */
	free(op->path);
	free(op);
    free(cls[0]);
    free(cls);

    elapsed_time += MPI_Wtime();
    if(!id)
		printf("Elapsed time: %.09f\n", elapsed_time);

    MPI_Finalize();

    return 0;
}
