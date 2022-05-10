#ifndef VECTOR
#define VECTOR

#include <stdlib.h>
#include <stdio.h>

int* create_vector(int length);
void delete_vector(int* vector);

void populate_vector(int* vector, unsigned int seed,int range, int length);
void print_vector(int* vector, int length);

#endif