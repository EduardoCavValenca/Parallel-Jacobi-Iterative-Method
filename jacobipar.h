#ifndef JACOBIPAR
#define JACOBIPAR

#include "common.h"

#define T 8

//Executing the method, line by line
bool iterate_par(v_type** matrix_A, v_type* vec_B, v_type* vec_solution, int length, v_type tolerance, int LOG);

#endif