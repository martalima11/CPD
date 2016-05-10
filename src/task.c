#include "task.h"

void insert_task(task_pool tpool, int * task){
	task_pool aux, new_tpool;
	
	int task_size = sizeof(task);

	if(tpool == NULL){
		tpool = (task_pool) malloc(sizeof(struct _task_pool));
		tpool->task = (int *) malloc(task_size);
		memcpy(tpool->task, task, task_size);
		tpool->next = NULL;
		return;
	}
	
	/* Ordered insertion */
	for(aux = tpool;
		aux->next != NULL &&
			(aux->task[TASK_Mc] > task[TASK_Mc] ||
			(aux->task[TASK_Mc] == task[TASK_Mc] &&
			aux->task[TASK_mc] > task[TASK_mc]));
		aux = aux->next);

	new_tpool = (task_pool) malloc(sizeof(struct _task_pool));
	new_tpool->task = (int *) malloc(task_size);
	memcpy(new_tpool->task, task, task_size);
	new_tpool->next = aux->next;
	aux->next = new_tpool;

	return;
}

int get_task(task_pool tpool, int * buff){
	task_pool aux;
	int task_size;

	if(tpool == NULL)
		return -1;
	else{
		task_size = sizeof(tpool->task);
		memcpy(buff, tpool->task, task_size);
		
		aux = tpool;
		tpool = tpool->next;
		free(aux->task);
		free(aux);
		
		return 0;
	}
}
