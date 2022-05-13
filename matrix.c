#include "matrix.h"

v_type** create_matrix(int rows, int cols){
    //Allocate matrix in heap

    v_type** matrix;
    matrix = (v_type **)malloc(rows * sizeof(v_type*));

    int i;
    for(i = 0; i < rows; i++) 
        matrix[i] = (v_type *)malloc(cols * sizeof(v_type));

    return matrix;

}

void delete_matrix(v_type** matrix, int rows, int cols){
    //free matrix memory in heap

    int i;
    for(i = 0; i < rows; i++) 
        free(matrix[i]);

    free(matrix);   
}


void populate_matrix(v_type** matrix, unsigned int seed, int range, int rows, int cols ){
    srand(seed); //Define seed

    int i,j;
    v_type line_sum;

    for (i = 0; i < rows ; i++){
        line_sum = 0;
        for (j = 0; j < cols ; j++){
            if(i != j){
                matrix[i][j] = (float)rand()/(float)(RAND_MAX/range); //random floating numbers between 0 and range
                line_sum += fabs(matrix[i][j]);
            }
        }
        matrix[i][i] = line_sum + (float)rand()/(float)(RAND_MAX/range); //Converging requirement
    }
}

void print_matrix(v_type** matrix, int rows, int cols){

    int i,j;
    
    for (i = 0; i < rows ; i++){
        for (j = 0; j < cols ; j++){
            printf("%lf ",matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}