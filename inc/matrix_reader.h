#ifndef MATRIX_READER_INCLUDED
#define MATRIX_READER_INCLUDED

#include <stdio.h>

typedef struct 
{
	float ** value;
	int order;
} matrix_t;

typedef struct
{
	float * value;
	int order;
} vector_t;

typedef struct
{
	int ITE_MAX;
	int ROW_TEST;
	float ERROR;
	matrix_t MATRIX;
	vector_t VECTOR;
} TaskDescriptor;

TaskDescriptor * readTask (FILE * input);
void freeTask(TaskDescriptor ** task);
void printMatrix(matrix_t * mat, FILE * output);
void printVector(vector_t * vec, FILE * output);



#endif