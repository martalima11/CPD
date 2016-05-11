#ifndef __TASK_H__
#define __TASK_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TASK_Mc    0
#define TASK_mc    1
#define TASK_level 2
#define TASK_vars  3

#define TASK_max  0
#define TASK_nmax 1
#define TASK_maxpath 2



typedef struct _task_pool{
	int * task;
	struct _task_pool * next;
} * task_pool;

void insert_task(task_pool *tpool, int * task, int task_size);
int get_task(task_pool *tpool, int * buff, int task_size);
 
#endif

