#include "vector.h"

int* create_vector(int length){
    int* vector;
    vector = (int *)malloc(length * sizeof(int));   

    return vector;

}

void delete_vector(int* vector){
    free(vector);   
}


void populate_vector(int* vector, unsigned int seed, int range, int length){
    srand(seed);

    int i;

    for (i = 0; i < length ; i++)
        vector[i] = rand() % range;
}

void print_vector(int* vector, int length){

    int i;

    for (i = 0; i < length ; i++)
        printf("%d ", vector[i]);

    printf("\n");
    
}