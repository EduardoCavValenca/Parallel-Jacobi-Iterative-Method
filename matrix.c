#include "matrix.h"

int** create_matrix(int rows, int cols){
    int** matrix;
    matrix = (int **)malloc(rows * sizeof(int*));

    int i;
    for(i = 0; i < rows; i++) 
        matrix[i] = (int *)malloc(cols * sizeof(int));

    return matrix;

}

void delete_matrix(int** matrix, int rows, int cols){

    int i;
    for(i = 0; i < rows; i++) 
        free(matrix[i]);

    free(matrix);   
}


void populate_matrix(int** matrix, unsigned int seed, int range, int rows, int cols ){
    srand(seed);

    int i,j;

    for (i = 0; i < rows ; i++){
        for (j = 0; j < cols ; j++){
            matrix[i][j] = rand()%range;
        }
    }
}

void print_matrix(int** matrix, int rows, int cols){

    int i,j;

    for (i = 0; i < rows ; i++){
        for (j = 0; j < cols ; j++){
            printf("%d ",matrix[i][j]);
        }
        printf("\n");
    }
}