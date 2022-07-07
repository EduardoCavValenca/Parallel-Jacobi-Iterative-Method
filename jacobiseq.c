/*

SSC0903 - Grupo 07
Eduardo Cavalari Valença 11234381
Che Fan Pan 11200421
Marcos Vinícius Firmino Pietrucci 10770072

*/

#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define RANGE 10
#define TOLERANCE 1e-7
#define LOG 1
#define LOG_MAX 10
#define SEED 10
#define QUESTION 0

void populate_matrix(double** matrix, int range, int rows, int cols);
void recalculate_matrix(double** matrix, int range, int rows, int cols, double *vec_B);
void populate_vector(double* vector, int range, int N);
double** create_matrix(int rows, int cols);
void delete_matrix(double** matrix, int rows, int cols);
void print_matrix(double** matrix, int rows, int cols);
double* create_vector(int length);
void delete_vector(double* vector);
void print_vector(double* vector, int length);
void initial_approximation(double* vec_solution, int length);
void verify_method(double** matrix_A, double* vec_B, double* vec_solution, int length,int line);

//Struct of variables to count the time
typedef struct {
    double start_total;
    double end_total;

    double start_matrix;
    double end_matrix;

    double start_iteration;
    double end_iteration;

}time;

int main (int argv, char **argc) {

    time times; //Struct to save time taken at each step
    times.start_total = omp_get_wtime(); //Start Timer of total runtime

    //Check if input is correct
    if(argv != 2)
    {
        printf("Starting values are wrong");
        exit(0);
    }
    
    int N;
    N = atoi(argc[1]); //Size of matrix

    //Creating the matrix and vectors
    double ** matrix_A = create_matrix(N,N);
    double * vec_B = create_vector(N);
    double * vec_solution = create_vector(N);

    srand(SEED); //Define seed

    //Pseudorandom number generation
    populate_vector(vec_B, RANGE, N);

    times.start_matrix = omp_get_wtime();
    populate_matrix(matrix_A, RANGE, N, N); //Pseudorandom number generation
    times.end_matrix = omp_get_wtime();

    //Divide by matrix_A and vec_B by main diagonal elements, and zero them
    recalculate_matrix(matrix_A, RANGE, N, N,vec_B);

    /*Presenting initial values*/
    if(LOG && N <= LOG_MAX){
        printf("Matrix: \n");
        print_matrix(matrix_A,N,N);

        printf("Target Vector: \n");
        print_vector(vec_B,N);
    }

    //Starting guess, current 0, 0, 0... 0
    initial_approximation(vec_solution,N); 

    if(LOG && N <= LOG_MAX)
        printf("Solution (Current - Difference): \n");
    
    //Starting the jacobi method
    int iteration_counter;

    times.start_iteration = omp_get_wtime();

    //Variables to iterate
    double max_diff; //Max diff between x(k) and x(k+1)
    double max_vec;  //Max value of x_i
    double stop_condition = DBL_MAX; //Tolerance
    int i,j = 0;
    double soma;
    double sum[N]; //Get line sum of Ax excluding Aii
    double change[N]; //Diference between value of past and new generation, stop condition
    double new_value; //New Xi value

    //######################### Iterations ###############################
    for(iteration_counter = 0; stop_condition > TOLERANCE; iteration_counter++){

        //For each line
        for(i=0; i < N; i++){ 
            
            soma = 0;
            //For each collum
            for(j = 0; j < N; j++)
                soma += matrix_A[i][j]*vec_solution[j]; //Sum of line
            sum[i] = soma;
        }
        
        //Applying the Jacobi method
        for(i= 0 ; i < N ; i++){
            new_value = (vec_B[i] - sum[i]); //Jacobi Equation
            change[i] = fabs(vec_solution[i] - new_value);
            vec_solution[i] = new_value; //updates value
        }

        /* ----------- Verifying the stop condition ---------------- */
        
        max_diff = 0; //Maximum change of this iteration
        max_vec = 0; //Maximum value of vector of Xi

        for(i= 0 ; i < N ; i++) 
            max_diff = fmax(max_diff, fabs(change[i]));

        for(i= 0 ; i < N ; i++) 
            max_vec = fmax(max_vec, fabs(vec_solution[i]));

        stop_condition = max_diff/max_vec; //Stop condition

        if(LOG && N <= LOG_MAX){
            for(i= 0 ; i < N ; i++)
                printf("%lf-%lf   ",vec_solution[i],change[i]);

            printf("stop: %lf", stop_condition);
            printf("\n");
        }
    }
    //####################### End of Iterations #############################
    times.end_iteration = omp_get_wtime();
    times.end_total = omp_get_wtime();

    if(LOG && N <= LOG_MAX){
        printf("\nSolution: (%d iterations)\n",iteration_counter);
        print_vector(vec_solution,N); //Final solution
    }

    // printf("Time taken build matrix: %lf\n", times.end_matrix - times.start_matrix);
    // printf("Time taken iterating: %lf\n",  times.end_iteration - times.start_iteration);
    // printf("Total time: %lf\n",  times.end_total - times.start_total);

    printf("Number of Iterations: %d\n\n", iteration_counter);
    printf("Time taken iterating: %lf\n",  times.end_iteration - times.start_iteration);

    if (QUESTION) {
        int search_line = 0;
        while(search_line < 1 || search_line > N){
            printf("\nDigite a equacao desejada 1 - N: ");
            fflush(0);
            scanf("%d", &search_line);
        }

        verify_method(matrix_A,vec_B,vec_solution,N,search_line-1);
    }

    //Free memory
    delete_matrix(matrix_A,N,N); 
    delete_vector(vec_B);
    delete_vector(vec_solution);

    return 0;
}

//Generate random double number between -1 and 1
double randomNumber() {
  return (double) rand()/RAND_MAX * 2.0 - 1.0;
}

void populate_matrix(double** matrix, int range, int rows, int cols){
  
    double line_sum = 0.0;
    double line_coefficient;

    for (int i = 0; i < rows; i++) {
        
        matrix[i][i] = randomNumber();

        line_sum = 0.0;

        for (int j = 0; j < cols; j++) {
            if (i != j){
                matrix[i][j] = randomNumber();
                line_sum += fabs(matrix[i][j]); //Get sum of line
            }
        }

        // Main diagonal coefficient > line_sum of same row
        line_coefficient = line_sum / fabs(matrix[i][i]) + fabs(randomNumber());

        for (int j = 0; j < cols; j++) {
            if (i != j)
                matrix[i][j] /= line_coefficient; //Make main diagonal elements greater than line sum
        }
    }
}

void recalculate_matrix(double** matrix, int range, int rows, int cols, double *vec_B){
    int i,j;

    //Updates matrix values
    for (i = 0; i < rows ; i++){
        for (j = 0; j < cols ; j++){
            if(i != j)
                matrix[i][j] /=  matrix[i][i];
        }
        vec_B[i] /=  matrix[i][i]; //Divide elements by main diagonal element
        matrix[i][i] = 0; //Zero the element so we dont need to check on the iteration sum
    }
}


void populate_vector(double* vector, int range, int N){

    int i;

    for (i = 0; i < N ; i++)
        vector[i] = randomNumber(); //random doubles numbers

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

void initial_approximation(double* vec_solution, int N){
    //Starting guess, current 0, 0, 0... 0 
    int i;

    //Add zeros the vec_solutions
    for (i= 0 ; i < N ; i++)
        vec_solution[i] = 0;
}


void verify_method(double** matrix_A, double* vec_B, double* vec_solution, int N, int line){
    //Verify difference between expected and founded value.

    int j;
    double diff, sum;

    sum = 0;
    for(j=0; j< N; j++){
        if(line == j) 
            sum += vec_solution[j]; //Main diagonal elements aii = 1, but 0 on the matrix
        else 
            sum += matrix_A[line][j]*vec_solution[j];
    }
    diff = fabs(sum - vec_B[line]);
    printf("Valor obtido (Ax): %lf\n", sum);
    printf("Valor objetivo (b): %lf\n", vec_B[line]);
    printf("Diferenca: %lf\n", diff);


    printf("\n");
    
}