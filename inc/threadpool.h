/*
 * Copyright (c) 2015, Guilherme de Freitas Perinazzo
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THREADPOOL_INCLUDED
#define THREADPOOL_INCLUDED

#include <pthread.h>

/* A small threadpool implementation using pthread */

/**
  *	threadpool_future_t
  * An object that can be used to check if a work has been
  * executed
 **/
typedef struct
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	char complete_flag;
} threadpool_future_t;

/**
  * threadpool_work_t
  * Defines a work to be executed by a worker thread
  * The argument pointer is passed to the work_func
  * If promise is set, the worker will set the promise
  * flag and signal any waiting threads once the work is complete
 **/
typedef struct
{
	void (*work_func)(void*);
	void * argument;
	threadpool_future_t * promise;
} threadpool_work_t;

/**
  * threadpool_queue_node_t
  * A queue node for the threadpool's internal queue
  * For Internal use only
 **/
typedef struct threadpool_queue_node_t_
{
	threadpool_work_t value;
	struct threadpool_queue_node_t_ * next;
} threadpool_queue_node_t;

/**
  * threadpool_queue_t
  * A single-linked queue used for passing
  * work into the worker threads
  * For Internal use only
 **/
typedef struct
{
	threadpool_queue_node_t *head, *tail_copy, *first;
	threadpool_queue_node_t *tail;
	pthread_mutex_t queue_mutex;
	pthread_cond_t queue_cond;
	char shutdown_flag;
} threadpool_queue_t;

/**
  * threadpool_worker_t
  * Worker thread definition
  * For Internal use only
 **/
typedef struct 
{
	pthread_t thread_id;

} threadpool_worker_t;

/**
  * threadpool_t
  * The threadpool structure
 **/
typedef struct 
{
	int num_workers;
	threadpool_worker_t * workers;
	threadpool_queue_t queue;

} threadpool_t;

/*
 * Function:  threadpool_future_init 
 * --------------------
 * Initializes (size) future structs. This should be called before their use.
 *
 *  future : A vector of threadpool_future_t to be initialized
 *  size   : Number of elements in future
 */
 void threadpool_future_init(threadpool_future_t * future, int size);

/*
 * Function:  threadpool_future_reuse 
 * --------------------
 *  Reset the internal flag so a future can be reused
 *
 *  future : A vector of threadpool_future_t to be reset
 *  size   : Number of elements in future
 */
 void threadpool_future_reuse(threadpool_future_t * future, int size);

/*
 * Function:  threadpool_future_waiton 
 * --------------------
 *  Waits on a list of futures. The function will return once all works associated
 *  with the futures have been completed. The thread will yield.
 *
 *  future : A vector of threadpool_future_t to be waited on
 *  size   : Number of elements in future
 */
void threadpool_future_waiton(threadpool_future_t * future, int size);

/*
 * Function:  threadpool_future_destroy 
 * --------------------
 *  Destroy a list of futures, releasing internal resources
 *
 *  future : A vector of threadpool_future_t to be destroyed
 *  size   : Number of elements in future
 */
void threadpool_future_destroy(threadpool_future_t * future, int size);

/*
 * Function:  threadpool_work_init 
 * --------------------
 *  Initializes a list of works. Not required to use the works.
 *
 *  works : A vector of threadpool_work_t to be initialized
 *  size  : Number of elements in works
 */
void threadpool_work_init(threadpool_work_t * works, int size);

/*
 * Function:  threadpool_init 
 * --------------------
 *  Initializes a threadpool, allocating necessary memory and
 *  initiating the worker threads. The threadpool is ready to be used
 *  once this returns. 
 *
 *  pool        : threadpool to be initialized 
 *  num_workers : Number of worker threads to be created
 */
void threadpool_init(threadpool_t * pool, int num_workers);

/*
 * Function:  threadpool_add_work_batch 
 * --------------------
 *  Inserts works into the threadpool's queue to be executed by the worker threads
 *
 *  pool  : threadpool to add the works
 *  works : A vector of threadpool_work_t to be added to the threadpool queue
 *  size  : Number of elements in works
 */
void threadpool_add_work_batch(threadpool_t * pool, threadpool_work_t * works, int size);

/*
 * Function:  threadpool_destroy 
 * --------------------
 *  Stop worker threads and release all resources used by the threadpool
 *  Any work still on the queue will NOT be completed.
 *  Any started work will be finished.
 *
 *  pool        : threadpool to be initialized 
 */
void threadpool_destroy(threadpool_t * pool);

#endif