#include "task.h"

/* Function used to insert a task on the list
 * Inserts tasks ordered by possible calculated maximum */
void insert_task(task_pool *tpool, int * task, int task_size){
	task_pool aux, new_tpool;
	if((*tpool) == NULL){
		(*tpool) = (task_pool) malloc(sizeof(struct _task_pool));
		(*tpool)->task = (int *) malloc(task_size*sizeof(int));
		copy_task((*tpool)->task, task, task_size);
		(*tpool)->next = NULL;

		return;
	}
	/* Ordered insertion */
	for(aux = (*tpool);
		aux->next != NULL &&
			(aux->task[TASK_Mc] > task[TASK_Mc] ||
			(aux->task[TASK_Mc] == task[TASK_Mc] &&
			aux->task[TASK_mc] > task[TASK_mc]));
		aux = aux->next);

	new_tpool = (task_pool) malloc(sizeof(struct _task_pool));
	new_tpool->task = (int *) malloc(task_size*sizeof(int));
	copy_task(new_tpool->task, task, task_size);
	new_tpool->next = aux->next;
	aux->next = new_tpool;

	return;
}

void copy_task(int *dst, int *src, int size){
	int i;
	for(i = 0; i < size; i++)
		dst[i] = src[i];
}

/* Function used to get task from the list
 * The task is then removed from memory */
int get_task(task_pool *tpool, int *buff, int task_size, int max){
	task_pool aux;
	int i = 0;
	while((*tpool) != NULL){
		if((*tpool)->task[TASK_Mc] >= max){ // if task has a maximum worth calculating
			copy_task(buff, (*tpool)->task, task_size);
			i = 1;
		}

		aux = (*tpool);
		(*tpool) = (*tpool)->next;
		free(aux->task);
		free(aux);

		if(i) return 0;
	}

	return -1;
}

void print_task(int * task, int size){
	int i;
	
	printf("Mc: %d\t", task[0]);
	printf("mc: %d\t", task[1]);
	printf("level: %d\t", task[2]);
	for(i = 3; i < size; i++){
		printf("%d ", task[i]);
	}
	
	printf("\n");
}

