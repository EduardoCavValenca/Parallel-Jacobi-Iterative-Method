#ifndef MATRIX
#define MATRIX

#include <stdlib.h>
#include <stdio.h>

int** create_matrix(int rows, int cols);
void delete_matrix(int** matrix, int rows, int cols);

void populate_matrix(int** matrix, unsigned int seed,int range, int rows, int cols );
void print_matrix(int** matrix, int rows, int cols);

#endif