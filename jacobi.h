#ifndef JACOBI
#define JACOBI

#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void initial_approximation(v_type* vec_solution, int length);
bool iterate(v_type** matrix_A, v_type* vec_B, v_type* vec_solution, int length, v_type tolerance);
void verify_method(v_type** matrix_A, v_type* vec_B, v_type* vec_solution, int length);
bool check_if_possible(v_type** matrix_A, int length);


#endif