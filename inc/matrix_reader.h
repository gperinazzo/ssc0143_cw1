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

#ifndef MATRIX_READER_INCLUDED
#define MATRIX_READER_INCLUDED

#include <stdio.h>

/**
  * matrix_t
  * Holds a square matrix with (order)x(order) elements
 **/
typedef struct 
{
	double ** value;
	int order;
} matrix_t;

/**
  * vector_t
  * Holds a vector with (order) elements
 **/
typedef struct
{
	double * value;
	int order;
} vector_t;

/**
  * TaskDescriptor
  * Holds all information from the input
  * that describe the task to be performed
 **/
typedef struct
{
	int ITE_MAX;
	int ROW_TEST;
	double ERROR;
	matrix_t MATRIX;
	vector_t VECTOR;
} TaskDescriptor;

/*
 * Function:  readTask 
 * --------------------
 * Creates a TaskDescriptor and reads all necessary information about the task
 * from input.
 *
 *  input   : File to read the task from
 *  Returns : Created TaskDescriptor
 */
TaskDescriptor * readTask (FILE * input);

/*
 * Function:  freeTask 
 * --------------------
 * Free all the memory used by a TaskDescriptor, and the TaskDescriptor struct.
 *
 *  task : TaskDescriptor to be freed
 */
void freeTask(TaskDescriptor ** task);


#endif