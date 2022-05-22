#ifndef JACOBISEQ
#define JACOBISEQ

#include "common.h"

//Executing the method, line by line
bool iterate_seq(v_type** matrix_A, v_type* vec_B, v_type* vec_solution, int length, v_type tolerance);

#endif