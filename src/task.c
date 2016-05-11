#include "task.h"

void print_pool(task_pool tpool){
	task_pool aux;
	printf("---------------------------------------\n");
	if(aux == NULL)
		printf("Welcome to the void!1!\n");
	for(aux = tpool; aux != NULL; aux = aux->next)
		printf("Mc: %d; mc: %d; Level: %d\n", aux->task[TASK_Mc], aux->task[TASK_mc], aux->task[TASK_level]);
	printf("---------------------------------------\n");
}

void insert_task(task_pool *tpool, int * task, int task_size){
	task_pool aux, new_tpool;
	int i;
	if((*tpool) == NULL){
		(*tpool) = (task_pool) malloc(sizeof(struct _task_pool));
		(*tpool)->task = (int *) malloc(task_size);
		/*memcpy(tpool->task, task, task_size);*/
		for(i=0;i<task_size; i++)
			(*tpool)->task[i] = task[i];
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
	new_tpool->task = (int *) malloc(task_size);
	/*memcpy(new_tpool->task, task, task_size);*/
	for(i=0;i<task_size; i++)
		new_tpool->task[i] = task[i];
	new_tpool->next = aux->next;
	aux->next = new_tpool;
	
	return;
}

int get_task(task_pool *tpool, int * buff, int task_size){
	task_pool aux;
	int i;
	if((*tpool) == NULL)
		return -1;
	else{
		/*memcpy(buff, tpool->task, task_size);*/
		
		for(i=0;i<task_size; i++)
			buff[i] = (*tpool)->task[i];
		
		aux = (*tpool);
		(*tpool) = (*tpool)->next;
		free(aux->task);
		free(aux);
		
		return 0;
	}
}

