#include "task.h"

void insert_task(task_pool tpool, int * task){
	task_pool aux, new_task;

	if(tpool == NULL){
		tpool = (task_pool) malloc(sizeof(struct _task_pool));
		tpool->task = task;
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
	new_tpool->task = task;
	new_tpool->next = aux->next;
	aux->next = new_tpool;

	return;
}

int * get_task(task_pool tpool){
	task_pool aux;
	int * task;

	if(tpool == NULL)
		task = NULL;
	else{
		task = tpool->task;
		aux = tpool;
		tpool = tpool->next;
		free(aux);
	}

	return task;
}
