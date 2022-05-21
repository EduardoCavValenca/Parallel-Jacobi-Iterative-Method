#ifndef JACOBISEQ
#define JACOBISEQ

#include "common.h"

//Add the initial guess (0,0,0,.... by default)
void initial_approximation(v_type* vec_solution, int length);

//Executing the method, line by line
bool iterate(v_type** matrix_A, v_type* vec_B, v_type* vec_solution, int length, v_type tolerance);

//Function that verify how precise the method is.
//Multiply the matrix by vec_solutions and compare it to vec_B
void verify_method(v_type** matrix_A, v_type* vec_B, v_type* vec_solution, int length);

//Check if the generated matrix supports the jacobi method
bool check_if_possible(v_type** matrix_A, int length);

#endif