#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define RANGE 1.0
#define TOLERANCE 1e-5
#define LOG 1

void populate_matrix(double** matrix, unsigned int seed, int range, int rows, int cols );
void populate_vector(double* vector, unsigned int seed, int range, int N);
double** create_matrix(int rows, int cols);
void delete_matrix(double** matrix, int rows, int cols);
void print_matrix(double** matrix, int rows, int cols);
double* create_vector(int N);
void delete_vector(double* vector);
void print_vector(double* vector, int N);
void initial_approximation(double* vec_solution, int N);
void verify_method(double** matrix_A, double* vec_B, double* vec_solution, int N);


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

    if(argv != 3)
    {
        printf("Starting values are wrong");
        exit(0);
    }
    
    int N, T;
    N = atoi(argc[1]);
    T = atoi(argc[2]);
   
    //Creating the matrix and vectors
    double ** matrix_A = create_matrix(N,N);
    double * vec_B = create_vector(N);
    double * vec_solution = create_vector(N);

    times.start_matrix = omp_get_wtime();
    populate_matrix(matrix_A, 100, RANGE, N, N); //Pseudorandom number generation
    times.end_matrix = omp_get_wtime();

    //Pseudorandom number generation
    populate_vector(vec_B, 100, RANGE, N);

    /*Presenting initial values*/
    if(LOG && N <= 10){
        printf("Matrix: \n");
        print_matrix(matrix_A,N,N);

        printf("Target Vector: \n");
        print_vector(vec_B,N);
    }

    //Starting guess, current 0, 0, 0... 0
    initial_approximation(vec_solution,N); 

    if(LOG && N <= 10)
        printf("Solution (Current - Difference): \n");
    
    //Starting the jacobi method
    int iteration_counter;

    times.start_iteration = omp_get_wtime();

    //Variables to iterate
    double max_diff = DBL_MAX;
    int i,j;
    double soma;
    double sum[N]; //Get line sum of Ax excluding Aii
    double change[N]; //Diference between value of past and new generation, stop condition
    double new_value; //New Xi value

    //######################### Iterations ###############################
    for(iteration_counter = 0; max_diff >= TOLERANCE; iteration_counter++){

        //For each line
        #pragma omp parallel for private(soma,j) num_threads(T) //schedule(dynamic,1)
        for(i=0; i < N; i++){ 
            
            soma = 0;
            //For each collum
            for(j = 0; j < N; j++){
                //If the element is not on the diagonal, add it to the total sum
                if(i != j)
                    soma += matrix_A[i][j]*vec_solution[j];
            }
            sum[i] = soma;
        }
        
        //Applying the Jacobi method
        #pragma omp parallel for if(N>500) private(new_value) num_threads(T)
        for(i= 0 ; i < N ; i++){
            new_value = (vec_B[i] - sum[i])/matrix_A[i][i]; //Jacobi Equation
            change[i] = fabs(vec_solution[i] - new_value);
            vec_solution[i] = new_value; //updates value
        }

       if(LOG && N <= 10){
            for(i= 0 ; i < N ; i++)
                printf("%lf-%lf   ",vec_solution[i],change[i]);

            printf("\n");
        }

        /* ----------- Verifying the stop condition ---------------- */
        
        max_diff = 0; //Maximum change of this iteration

        #pragma omp parallel for reduction(max : max_diff) num_threads(T)
        for(i= 0 ; i < N ; i++) 
            max_diff = fmax(max_diff, change[i]);
    }
    //####################### End of Iterations #############################
    times.end_iteration = omp_get_wtime();
    times.end_total = omp_get_wtime();

    if(LOG && N <= 10){
        printf("\nSolution: (%d iterations)\n",iteration_counter);
        print_vector(vec_solution,N); //Final solution

        printf("Diference from target: \n"); //Diference between Ax and b where (Ax = b)
        verify_method(matrix_A,vec_B,vec_solution, N);
    }

    // printf("Time taken build matrix: %lf\n", times.end_matrix - times.start_matrix);
    // printf("Time taken iterating: %lf\n",  times.end_iteration - times.start_iteration);
    // printf("Total time: %lf\n",  times.end_total - times.start_total);
    // printf("Number of Iterations: %d\n\n", iteration_counter);

    printf("Time taken iterating: %lf\n",  times.end_iteration - times.start_iteration);

    //Free memory
    delete_matrix(matrix_A,N,N); 
    delete_vector(vec_B);
    delete_vector(vec_solution);

    return 0;
}


void populate_matrix(double** matrix, unsigned int seed, int range, int rows, int cols ){
    srand(seed); //Define seed

    int i,j;
    double line_sum;

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


void populate_vector(double* vector, unsigned int seed, int range, int N){

    int i;

    for (i = 0; i < N ; i++)
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


double* create_vector(int N){
    //Allocate vector in heap
    
    double* vector;
    vector = (double *)malloc(N * sizeof(double));   

    return vector;

}

void delete_vector(double* vector){
    //free vector memory in heap
    free(vector);   
}


void print_vector(double* vector, int N){

    int i;

    for (i = 0; i < N ; i++)
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


void verify_method(double** matrix_A, double* vec_B, double* vec_solution, int N){
    //Verify difference between expected and founded value.

    int i, j;

    double diff, sum;

    for(i=0; i< N; i++)
    {
        sum = 0;
        for(j=0; j< N; j++){
            sum += matrix_A[i][j]*vec_solution[j];
        }
        diff = fabs(sum - vec_B[i]);
        printf("%lf ", diff);
    }
    printf("\n\n");

}
