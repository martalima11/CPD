#ifndef __TASK_H__
#define __TASK_H__

typedef struct _task_pool{
	int * task;
	struct _task_pool * next;
} * task_pool;

void insert_task(task_pool tpool, int * task);
int * get_task(task_pool tpool);
 

#endif

