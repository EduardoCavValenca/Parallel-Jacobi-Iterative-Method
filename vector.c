#include "vector.h"

v_type* create_vector(int length){
    //Allocate vector in heap
    
    v_type* vector;
    vector = (v_type *)malloc(length * sizeof(v_type));   

    return vector;

}

void delete_vector(v_type* vector){
    //free vector memory in heap
    free(vector);   
}


void populate_vector(v_type* vector, unsigned int seed, int range, int length){
    srand(seed);//Define seed

    int i;

    for (i = 0; i < length ; i++)
        vector[i] = (float)rand()/(float)(RAND_MAX/range); //random floating numbers between 0 and range
}

void print_vector(v_type* vector, int length){

    int i;

    for (i = 0; i < length ; i++)
        printf("%f ", vector[i]);

    printf("\n\n");
    
}