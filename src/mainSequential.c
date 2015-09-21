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
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "matrix_reader.h"

void swap_vectors (float ** v1, float ** v2)
{
	float * tmp = *v1;
	*v1 = *v2;
	*v2 = tmp;
}

int main(int argc, char ** argv)
{
	int i, j, counter, order;
	float * current = NULL, * old = NULL, * test = NULL, sum_test;
	
	// Setup:
	// TaskDescriptor has all the info read from the input
	TaskDescriptor * task = readTask(stdin);
	current = malloc(sizeof(float) * task->VECTOR.order);
	old = malloc(sizeof(float) * task->VECTOR.order);
	test = malloc (sizeof(float) * (task->VECTOR.order + 1) );

	// test vector saves the original values of the matrix row and the value required from the vector for testing
	// after the algorithm has ended.
	// Since we only test one row, this is a good approach to save memory
	memcpy(test, task->MATRIX.value[task->ROW_TEST], sizeof(float) * task->MATRIX.order);
	test[task->VECTOR.order] = task->VECTOR.value[task->ROW_TEST];

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
		float max_dif, max_value, error, value;
		swap_vectors(&old, &current);

		for (i = 0; i < order; ++i)
		{
			float sum = 0.0;
			for (j = 0; j < order; ++j)
			{
				// We know value[i][i] is 0, it won't change the value
				// Faster to do 1 mult and 1 addition once than an if for every value
				sum += (task->MATRIX.value[i][j] * old[j]);
			}
			current[i] = task->VECTOR.value[i] - sum;
		}

		// We have ended the iteration, we increase the counter here
		// so that if we break out of the for loop the counter will be correct
		++counter;

		// Calculate the error
		// Its calculated as
		// error_i = Max(Dif[0], Dif[1], ... , Dif[n-1]) / Max(Abs(x[0]_i, x[1]_i, ..., x[n-1]_i))
		// where Dif[i] = Abs(x[i]_k - x[i]_(k-1))
		max_dif = fabs(current[0] - old[0]);
		max_value = fabs(current[0]);
		for (i = 1; i < order; ++i)
		{
			value = fabs(current[i] - old[i]);
			if (value > max_dif) max_dif = value;
			value = fabs(current[i]);
			if (value > max_value) max_value = value;
		}

		error = max_dif/max_value;
		if (error < task->ERROR)
			break;


	}

	/* End of the algorithm */

	/* Tests and output */
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

	return 0;
}