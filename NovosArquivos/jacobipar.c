#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>


#define RANGE 1.0
#define TOLERANCE 1e-5
#define LOG 0

void populate_matrix(double** matrix, unsigned int seed, int range, int rows, int cols );
void populate_vector(double* vector, unsigned int seed, int range, int length);
double** create_matrix(int rows, int cols);
void delete_matrix(double** matrix, int rows, int cols);
void print_matrix(double** matrix, int rows, int cols);
double* create_vector(int length);
void delete_vector(double* vector);
void print_vector(double* vector, int length);


typedef struct
{
    double start_total = 0;
    double end_total = 0;

    double start_matrix = 0;
    double end_matrix = 0;

    double start_iteration = 0;
    double end_iteration = 0;

} time;


int main () {

    int N;
    printf("Digite o tamanho da matrix: \n");
    scanf("%d", &N); //Size of matrix

    int T;
    printf("Digite o numero de threads: \n");
    scanf("%d", &T); //Size of matrix

    //Creating the matrix and vectors
    double ** matrix_A = create_matrix(N,N);
    double * vec_B = create_vector(N);
    double * vec_solution = create_vector(N);




}


void populate_matrix(double** matrix, unsigned int seed, int range, int rows, int cols ){
    srand(seed); //Define seed

    int i,j;
    v_type line_sum;

    for (i = 0; i < rows ; i++){
        line_sum = 0;
        for (j = 0; j < cols ; j++){
            if(i != j){
                matrix[i][j] = (double)rand()/(double)(RAND_MAX/range); //random floating numbers between 0 and range
                line_sum += fabs(matrix[i][j]);
            }
        }
        matrix[i][i] = line_sum + (double)rand()/(double)(RAND_MAX/range); //Converging requirement
    }
}


void populate_vector(double* vector, unsigned int seed, int range, int length){

    int i;

    for (i = 0; i < length ; i++)
        vector[i] = (double)rand()/(double)(RAND_MAX/range); //random floating numbers between 0 and range
}


double** create_matrix(int rows, int cols){
    //Allocate matrix in heap

    double** matrix;
    matrix = (double **)malloc(rows * sizeof(double*));

    int i;
    for(i = 0; i < rows; i++) 
        matrix[i] = (double *)malloc(cols * sizeof(double));

    return matrix;

}

void delete_matrix(double** matrix, int rows, int cols){
    //free matrix memory in heap

    int i;
    for(i = 0; i < rows; i++) 
        free(matrix[i]);

    free(matrix);   
}

void print_matrix(double** matrix, int rows, int cols){

    int i,j;
    
    for (i = 0; i < rows ; i++){
        for (j = 0; j < cols ; j++){
            printf("%lf ",matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}


double* create_vector(int length){
    //Allocate vector in heap
    
    double* vector;
    vector = (double *)malloc(length * sizeof(double));   

    return vector;

}

void delete_vector(double* vector){
    //free vector memory in heap
    free(vector);   
}


void print_vector(double* vector, int length){

    int i;

    for (i = 0; i < length ; i++)
        printf("%lf ", vector[i]);

    printf("\n\n");
    
}