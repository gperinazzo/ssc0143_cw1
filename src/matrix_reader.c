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

#include "matrix_reader.h"
#include <stdio.h>
#include <stdlib.h>

TaskDescriptor * readTask (FILE * input)
{
	int i, j;
	size_t row_size;
	TaskDescriptor * td = malloc(sizeof(TaskDescriptor));

	if (td)
	{
		td->MATRIX.value = NULL;
		fscanf(input, "%d\n%d\n%f\n%d", &td->MATRIX.order, &td->ROW_TEST, &td->ERROR, &td->ITE_MAX);
		td->VECTOR.order = td->MATRIX.order;
		
		td->MATRIX.value = malloc(sizeof(float*) * td->MATRIX.order);

		row_size = sizeof(float) * td->MATRIX.order;

		td->VECTOR.value  = malloc(row_size);
		
		for (i = 0; i < td->MATRIX.order; ++i)
			td->MATRIX.value[i] = malloc(row_size);


		for (i = 0; i < td->MATRIX.order; ++i)
			for (j = 0; j < td->MATRIX.order; ++j)
				fscanf(input, "%f", &td->MATRIX.value[i][j]);


		for (i = 0; i < td->VECTOR.order; ++i)
			fscanf(input, "%f", &td->VECTOR.value[i]);



	}
	return td;
}

void freeTask(TaskDescriptor ** task)
{
	int i;
	int order = (*task)->MATRIX.order;
	if ((*task)->MATRIX.value)
	{
		for (i = 0; i < order; ++i)
			free ((*task)->MATRIX.value[i]);
		free ((*task)->MATRIX.value);
	}
	if ((*task)->VECTOR.value)
		free ((*task)->VECTOR.value);
	free ((*task));
	*task = NULL;
}
