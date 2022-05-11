#ifndef VECTOR
#define VECTOR

#include "common.h"
#include <stdlib.h>
#include <stdio.h>

v_type* create_vector(int length);
void delete_vector(v_type* vector);

void populate_vector(v_type* vector, unsigned int  seed,int range, int length);
void print_vector(v_type* vector, int length);

#endif