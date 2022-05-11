#ifndef MATRIX
#define MATRIX

#include "common.h"
#include <stdlib.h>
#include <stdio.h>

v_type** create_matrix(int rows, int cols);
void delete_matrix(v_type** matrix, int rows, int cols);

void populate_matrix(v_type** matrix, unsigned int seed,int range, int rows, int cols );
void print_matrix(v_type** matrix, int rows, int cols);

#endif