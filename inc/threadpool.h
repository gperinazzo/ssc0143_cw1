#ifndef THREADPOOL_INCLUDED
#define THREADPOOL_INCLUDED

#include <pthread.h>

typedef struct
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	char complete_flag;
} threadpool_future_t;

typedef struct
{
	void (*work_func)(void*);
	void * argument;
	threadpool_future_t * promise;
} threadpool_work_t;

typedef struct threadpool_queue_node_t_
{
	threadpool_work_t value;
	struct threadpool_queue_node_t_ * next;
} threadpool_queue_node_t;

typedef struct
{
	threadpool_queue_node_t *head, *tail_copy, *first;
	threadpool_queue_node_t *tail;
	pthread_mutex_t queue_mutex;
	pthread_cond_t queue_cond;
	char shutdown_flag;
} threadpool_queue_t;

typedef struct 
{
	pthread_t thread_id;

} threadpool_worker_t;

typedef struct 
{
	int num_workers;
	threadpool_worker_t * workers;
	threadpool_queue_t queue;

} threadpool_t;

void threadpool_future_init(threadpool_future_t * future, int size);
void threadpool_future_reuse(threadpool_future_t * future, int size);
void threadpool_future_waiton(threadpool_future_t * future, int size);
void threadpool_future_destroy(threadpool_future_t * future, int size);

void threadpool_work_init(threadpool_work_t * works, int size);

void threadpool_init(threadpool_t * pool, int num_workers);

void threadpool_add_work_batch(threadpool_t * pool, threadpool_work_t * works, int size);
void threadpool_destroy(threadpool_t * pool);

#endif