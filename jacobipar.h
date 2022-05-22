#ifndef JACOBIPAR
#define JACOBIPAR

#include "common.h"

//Executing the method, line by line
bool iterate_par(v_type** matrix_A, v_type* vec_B, v_type* vec_solution, int length, v_type tolerance);

#endif