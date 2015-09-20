#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "threadpool.h"

void threadpool_future_init(threadpool_future_t * future, int size)
{
	int i;
	for (i = 0; i < size; ++i)
	{
		pthread_mutex_init(&future[i].mutex, NULL);
		pthread_cond_init(&future[i].cond, NULL);
		future[i].complete_flag = 0;
	}
}

void threadpool_future_waiton(threadpool_future_t * future, int size)
{
	int i;
	for (i = 0; i < size; ++i)
	{
		pthread_mutex_lock(&future[i].mutex);
		while(!future[i].complete_flag)
		{	
			pthread_cond_wait(&future[i].cond, &future[i].mutex);
		}
		pthread_mutex_unlock(&future[i].mutex);
	}
}

void threadpool_queue_init(threadpool_queue_t * queue)
{
	pthread_mutex_init(&queue->queue_mutex, NULL);
	pthread_cond_init(&queue->queue_cond, NULL);
	queue->first = queue->head = queue->tail = queue->tail_copy = malloc(sizeof(threadpool_queue_node_t));
	queue->shutdown_flag = 0;	
}

static void * threadpool_worker_main(void * arg)
{
	threadpool_queue_t * queue = (threadpool_queue_t*)arg;
	threadpool_work_t current_work;
	while(1)
	{
		pthread_mutex_lock(&queue->queue_mutex);
		while(!queue->tail->next && !queue->shutdown_flag)
		{
			pthread_cond_wait(&queue->queue_cond, &queue->queue_mutex);
		}
		
		if (queue->shutdown_flag)
		{
			pthread_mutex_unlock(&queue->queue_mutex);
			break;
		}

		current_work = queue->tail->next->value;
		queue->tail = queue->tail->next;

		pthread_mutex_unlock(&queue->queue_mutex);

		current_work.work_func(current_work.argument);
		if (current_work.promise)
		{
			pthread_mutex_lock(&current_work.promise->mutex);
			current_work.promise->complete_flag = 1;
			pthread_mutex_unlock(&current_work.promise->mutex);
			pthread_cond_broadcast(&current_work.promise->cond);
		}
	}
	return NULL;
}

void threadpool_worker_start(threadpool_worker_t * workers, int size, threadpool_queue_t * queue)
{
	int i;
	for (i = 0; i < size; ++i)
	{
		if (pthread_create(&workers[i].thread_id, NULL, threadpool_worker_main, (void*)queue))
		{
			fprintf(stderr, "Threadpool: unable to start thread %d\n", i);
		}
	}
}

void threadpool_init(threadpool_t * pool, int num_workers)
{
	pool->num_workers = num_workers;
	pool->workers = malloc(sizeof(threadpool_worker_t) * num_workers);

	threadpool_queue_init(&pool->queue);
	pool->workers = malloc(sizeof(threadpool_worker_t) * num_workers);
	threadpool_worker_start(pool->workers, num_workers, &pool->queue);
}

static threadpool_queue_node_t * alloc_node(threadpool_queue_t * queue)
{
	threadpool_queue_node_t * node = NULL;
	if (queue->first != queue->tail)
	{
		node = queue->first;
		queue->first = queue->first->next;
		return node;
	}
	node = malloc(sizeof(threadpool_queue_node_t));
	return node;
}

void threadpool_add_work_batch(threadpool_t * pool, threadpool_work_t * works, int size)
{
	threadpool_queue_node_t *first = NULL, *last = NULL, * node = NULL;
	int i;
	if (size <= 0) return;

	// This could be done without locking by properly using memory fences and atomic stores
	// on queue->tail
	// Since using fences and atomic variables is outside the scope of this project, 
	// we decided to use the mutex
	pthread_mutex_lock(&pool->queue.queue_mutex);

	first = last = alloc_node(&pool->queue);
	first->value = works[0];
	first->next = NULL;

	for (i = 1; i < size; ++i)
	{
		node = alloc_node(&pool->queue);
		node->value = works[i];
		node->next = NULL;
		last->next = node;
		last = node;
	}

	pool->queue.head->next = first;
	pool->queue.head = last;

	pthread_mutex_unlock(&pool->queue.queue_mutex);

	// Wake sleeping worker threads
	if (size > 1)
		pthread_cond_broadcast(&pool->queue.queue_cond);
	else
		pthread_cond_signal(&pool->queue.queue_cond);


}

void threadpool_work_init(threadpool_work_t * works, int size)
{
	int i;
	for (i = 0; i < size; ++i)
	{
		works[i].work_func = NULL;
		works[i].argument = NULL;
		works[i].promise = NULL;
	}
}

void threadpool_future_reuse(threadpool_future_t * future, int size)
{
	int i;
	for(i = 0; i < size; ++i)
	{
		future[i].complete_flag = 0;
	}
}

static void threadpool_shutdown(threadpool_t * pool)
{
	int i;

	// Signas we're shutting down
	pthread_mutex_lock(&pool->queue.queue_mutex);
	pool->queue.shutdown_flag = 1;
	pthread_mutex_unlock(&pool->queue.queue_mutex);
	pthread_cond_broadcast(&pool->queue.queue_cond);

	// Join all worker threads into the main thread
	for (i = 0; i < pool->num_workers; ++i)
	{
		pthread_join(pool->workers[i].thread_id, NULL);
	}

}

static void threadpool_queue_destroy(threadpool_queue_t * queue)
{
	threadpool_queue_node_t * node, *next;
	node = queue->first;
	while (node)
	{
		next = node->next;
		free(node);
		node = next;
	}
	pthread_mutex_destroy(&queue->queue_mutex);
	pthread_cond_destroy(&queue->queue_cond);
}

void threadpool_destroy(threadpool_t * pool)
{
	threadpool_shutdown(pool);
	if(pool->workers) free(pool->workers);
	threadpool_queue_destroy(&pool->queue);
	pool->num_workers = 0;
}

void threadpool_future_destroy(threadpool_future_t * future, int size)
{
	int i;
	for (i = 0; i < size; ++i)
	{
		pthread_mutex_destroy(&future[i].mutex);
		pthread_cond_destroy(&future[i].cond);
	}
}