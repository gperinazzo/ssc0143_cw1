#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "matrix_reader.h"
#include "threadpool.h"

typedef struct
{
	float * current;
	float * old;
	matrix_t * matrix;
	vector_t * vector;
	int start, end;
	float max_dif, max_value;
} work_info;

void work_func(void * argument)
{
	int i, j, order; 
	float max_dif, max_value, value;
	work_info * info = (work_info*) argument;

	max_dif = -1.0f;
	max_value = -1.0f;
	order = info->matrix->order;

	for(i = info->start ; i < info->end; ++i)
	{
		float sum = 0.0f;
		for (j = 0; j < order; ++j)
		{
			sum += (info->matrix->value[i][j] * info->old[j]);
		}

		// if two threads share a current range in the same cache line
		// this write will be slowed down
		info->current[i] = info->vector->value[i] - sum;

		// calculate the difference, and check if this is
		// the thread's maximum values
		value = fabs(info->current[i]);
		if(value > max_value)
			max_value = value;
		value = fabs(info->current[i] - info->old[i]);
		if (value > max_dif)
			max_dif = value;
	}

	// Set the return values
	info->max_dif = max_dif;
	info->max_value = max_value;

}

void swap_pointer (void ** p1, void ** p2)
{
	void * aux = *p1;
	*p1 = *p2;
	*p2 = aux;
}

void swap_vectors (float ** v1, float ** v2)
{
	float * tmp = *v1;
	*v1 = *v2;
	*v2 = tmp;
}


int main(int argc, char ** argv)
{
	int i, j, order, num_threads = 4, counter =0, *workload, sum;
	float * current = NULL, * old = NULL, * test = NULL, sum_test;
	threadpool_t pool;
	threadpool_work_t * work_cur, *work_next, *work;
	threadpool_future_t * futures_cur, *futures_next, *futures;
	work_info *info, *info_cur, *info_next;
	TaskDescriptor * task;

	if (argc >= 2)
		num_threads = atoi(argv[1]);

	task = readTask(stdin);
	current = malloc(sizeof(float) * task->VECTOR.order);
	old = malloc(sizeof(float) * task->VECTOR.order);
	test = malloc (sizeof(float) * (task->VECTOR.order + 1) );
	workload = malloc(sizeof(int) * num_threads);

	sum = 0;

	// Separate the workload for each thread
	for(i = 0; i < num_threads; ++i)
	{
		workload[i] = task->VECTOR.order / num_threads;
		sum += workload[i];
	}

	// if order isnt divisible by num_threads
	// we need to distribute remaining values
	for (i = 0; sum < task->VECTOR.order; ++sum)
	{
		++workload[i];
		i = (i + 1) % num_threads;
	}



	// test vector saves the original values of the matrix row and the value required from the vector for testing
	// after the algorithm has ended.
	// Since we only test one row, this is a good approach to save memory
	memcpy(test, task->MATRIX.value[task->ROW_TEST], sizeof(float) * task->MATRIX.order);
	test[task->VECTOR.order] = task->VECTOR.value[task->ROW_TEST];

	// Initialize work definitions and futures
	work = malloc (sizeof(threadpool_work_t) * num_threads * 2);
	futures = malloc (sizeof(threadpool_future_t) * num_threads * 2);
	info = malloc(sizeof(work_info) * num_threads * 2);
	futures_next = futures;
	work_next = work;
	info_next = info; 

	work_cur = work + num_threads;
	futures_cur = futures + num_threads;
	info_cur = info + num_threads;

	threadpool_init(&pool, num_threads);
	threadpool_work_init(work, num_threads * 2);
	threadpool_future_init(futures, num_threads * 2);

	// Set up the work information
	// Since we have 2 * num_threads works, we can prepare both batches
	// Since information for each batch never actually changes (aside from the current and old
	// vectors that are swapped) we dont need to change any of this during the loop execution
	sum = 0;
	for (i = 0; i < num_threads; ++i)
	{
		work_cur[i].work_func = work_func;
		work_cur[i].argument = (void*)(info_cur + i);
		work_cur[i].promise = futures_cur + i;
		work_next[i].work_func = work_func;
		work_next[i].argument = (void*)(info_next + i);
		work_next[i].promise = futures_next + i;

		info_cur[i].current = current;
		info_cur[i].old = old;
		info_cur[i].matrix = &task->MATRIX;
		info_cur[i].vector = &task->VECTOR; 
		info_cur[i].start = sum;
		info_cur[i].end = sum + workload[i];

		// old and current are swapped because we will swap them in the loop
		info_next[i].current = old;
		info_next[i].old = current;
		info_next[i].matrix = &task->MATRIX;
		info_next[i].vector = &task->VECTOR; 
		info_next[i].start = sum;
		info_next[i].end = sum + workload[i];

		sum += workload[i];		
	}

	

	/* Start of the algorithm */

	// Initiate the matrix by dividing each value by the diagonal value and setting the diagonal value as 0
	for (i = 0; i < task->MATRIX.order; ++i)
	{
		int diagonal_value = task->MATRIX.value[i][i];
		for (j = 0; j < task->MATRIX.order; j++)
				task->MATRIX.value[i][j] /= diagonal_value;
		task->VECTOR.value[i] /=  diagonal_value;
		task->MATRIX.value[i][i] = 0;
	}
	memcpy (current, task->VECTOR.value, sizeof(float) * task->VECTOR.order);
	order = task->VECTOR.order;


	for (counter = 0; counter < task->ITE_MAX; )
	{
		float max_dif, max_value;
		// Swap vectors for the next iteration
		swap_vectors(&old, &current);
		swap_pointer((void**)&info_cur, (void**)&info_next);
		swap_pointer((void**)&work_cur, (void**)&work_next);
		swap_pointer((void**)&futures_cur, (void**)&futures_next);
		threadpool_add_work_batch(&pool, work_cur, num_threads);

		// Do some work on the next batch while the worker threads finish the iteration
		threadpool_future_reuse(futures_next, num_threads);

		// Wait until the works end
		threadpool_future_waiton(futures_cur, num_threads);

		// We have ended the iteration, we increase the counter here
		// so that if we break out of the for loop the counter will be correct
		++counter

		// This batch is complete, calculate the maximum error
		// This is a sequential procedure between parallel sections
		// but we have reduced it to O(num_threads)
		max_dif = info_cur[0].max_dif;
		max_value = info_cur[0].max_value;
		for (i = 1; i < num_threads; ++i)
		{
			if (info_cur[i].max_dif > max_dif)
				max_dif = info_cur[i].max_dif;
			if (info_cur[i].max_value > max_value)
				max_value = info_cur[i].max_value;
		}

		if (max_dif / max_value < task->ERROR)
			break; 
	}

	sum_test = 0.0;
	for (i = 0; i < order; ++i)
	{
		sum_test += current[i] * test[i];
	}

	printf("---------------------------------------------------------\n" 
		"Iterations: %d\n" 
		"RowTest: %d => [%f] =? %f\n"
		"---------------------------------------------------------\n",
		counter, task->ROW_TEST, sum_test, test[order]);


	freeTask(&task);
	if (current) free(current);
	if (old) free (old);
	if (test) free(test);
	if (workload) free(workload);
	if (work) free(work);

	threadpool_future_destroy(futures, num_threads * 2);
	if (futures) free (futures);
	if (info) free(info);

	threadpool_destroy(&pool);

	return 0;
}