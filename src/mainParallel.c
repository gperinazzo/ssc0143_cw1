#include <stdio.h>
#include <stdlib.h>
#include "threadpool.h"

void work_func(void * argument)
{
	fflush(stdout);
	int i = 0;
	int sum = 0;
	for (i = 0; i < 1000000; ++i)
		sum += i;

	printf ("Work ended\n");
}


int main(int argc, char ** argv)
{
	int i, num_threads = 4;
	threadpool_t pool;
	threadpool_init(&pool, num_threads);
	threadpool_work_t * work;
	threadpool_future_t * futures;
	threadpool_work_init(work, 10);
	threadpool_future_init(futures, 10);
	for (i = 0; i < 10; ++i)
	{
		work[i].work_func = &work_func;
		work[i].promise = (futures + i);
	}

	threadpool_add_work_batch(&pool, work, 10);

	threadpool_future_waiton(futures, 10);
	printf ("all work done\n");

	threadpool_destroy(&pool);
	threadpool_future_destroy(futures, 10);

	return 0;
}