#ifndef MATRIX
#define MATRIX

#include "common.h"


//Alocate matrix
v_type** create_matrix(int rows, int cols);

//Free matrix from heap
void delete_matrix(v_type** matrix, int rows, int cols);

//Add random values to the matrix according to the prerequisites of the jacobi method
void populate_matrix(v_type** matrix, unsigned int seed,int range, int rows, int cols );

void print_matrix(v_type** matrix, int rows, int cols);

#endif