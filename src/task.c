#include "task.h"

void insert_task(task_pool tpool, int * task){
	task_pool aux;
	
	if(tpool == NULL){
		tpool = (task_pool) malloc(sizeof(struct _task_pool));
		tpool->task = task;
		tpool->next = NULL;
		return;
	}
	for(aux = tpool; aux->next != NULL; aux = aux->next);
	
	aux->next = (task_pool) malloc(sizeof(struct _task_pool));
	aux->next->task = task;
	aux->next->next = NULL;
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
