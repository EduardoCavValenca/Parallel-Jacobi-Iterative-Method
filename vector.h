#ifndef VECTOR
#define VECTOR

#include "common.h"

//Allocate vector in heap
v_type* create_vector(int length);

//Free vector memory in heap
void delete_vector(v_type* vector);

//Insert random values
void populate_vector(v_type* vector, unsigned int  seed,int range, int length);

void print_vector(v_type* vector, int length);

#endif