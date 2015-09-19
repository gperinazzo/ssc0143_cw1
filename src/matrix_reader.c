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

void printMatrix(matrix_t * mt, FILE * output)
{
	int i, j;
	for (i = 0; i < mt->order; ++i)
	{
		printf("passou\n");
		fflush(output);
		for (j = 0; j < mt->order - 1; ++j)
			fprintf(output, "%f, ", mt->value[i][j]);
		fprintf(output, "%f\n", mt->value[i][j]);
	}
}

void printVector(vector_t * vec, FILE * output)
{
	int i;
	for (i = 0; i < vec->order - 1; ++i)
	{
		fprintf(output, "%f, ", vec->value[i]);
			fflush(output);
	}
	fprintf(output, "%f\n", vec->value[i]);

}