#ifndef __TASK_H__
#define __TASK_H__

/* Task:
 * Index	- Value - Size
 * 
 * 0		- Mc 	- 1
 * 1		- mc	- 1
 * 2		- level	- 1
 * 3		- vars	- nvar (ends in 0)
 *  
 * */

typedef struct _task_pool{
	int * task;
	struct _task_pool * next;
} * task_pool;

void insert_task(task_pool tpool, int * task);
int * get_task(task_pool tpool);
 

#endif

