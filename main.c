#include "matrix.h"
#include "vector.h"
#include "jacobi.h"
#include "jacobiseq.h"
#include "jacobipar.h"

#define RANGE 1
#define TOLERANCE 0.0001
#define LOG 0

int main(){
    
    int N;
    scanf("%d", &N); //Size of matrix

    //Creating the matrix and vectors
    v_type ** matrix_A = create_matrix(N,N);
    v_type * vec_B = create_vector(N);
    v_type * vec_solution = create_vector(N);

    double start_time_matrix = omp_get_wtime();
    populate_matrix(matrix_A,3,RANGE,N,N); //Pseudorandom number generation
    double time_matrix = omp_get_wtime() - start_time_matrix;

    //Check if solution is able to converge with Jacobi method
    if(!check_if_possible(matrix_A,N))
    { 
        printf("Method is not aplicable for this seed\n\n"); //Aii != 0  , Aii > sum(rest_of_line)
        return 0;
    }

    //Pseudorandom number generation
    populate_vector(vec_B,5,RANGE,N); 

    /*Presenting initial values*/
    if(LOG){
        printf("Matrix: \n");
        print_matrix(matrix_A,N,N);

        printf("Target Vector: \n");
        print_vector(vec_B,N);
    }

    //Starting guess, current 0, 0, 0... 0
    initial_approximation(vec_solution,N); 

    if(LOG)
        printf("Solution (Current - Difference): \n");
    
    //Starting the jacobi method
    int iteration_counter = 0;


    double start_time_iter = omp_get_wtime();

    //Iterating while the tolerance is not reached
    while(iterate_par(matrix_A,vec_B,vec_solution, N, TOLERANCE, LOG)) //while there is substantial improvement (> tolerance)
        iteration_counter += 1;

    double time_iter = omp_get_wtime() - start_time_iter;
    
    /* Presenting the results */
    if (LOG){
        printf("\nSolution: (%d iterations)\n",iteration_counter);
        print_vector(vec_solution,N); //Final solution

        printf("Diference from target: \n"); //Diference between Ax and b where (Ax = b)
        verify_method(matrix_A,vec_B,vec_solution, N);
    }

    printf("Time taken build matrix: %lf\n\n", time_matrix);
    printf("Time taken iterating: %lf\n\n", time_iter);

    //Free memory
    delete_matrix(matrix_A,N,N); 
    delete_vector(vec_B);
    delete_vector(vec_solution);
}