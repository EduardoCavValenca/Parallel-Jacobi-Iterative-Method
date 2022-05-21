#include "matrix.h"
#include "vector.h"
#include "jacobi.h"

#define RANGE 1
#define TOLERANCE 0.000001

int main(){
    
    int N;
    scanf("%d", &N); //Size of matrix

    //Creating the matrix and vectors
    v_type ** matrix_A = create_matrix(N,N);
    v_type * vec_B = create_vector(N);
    v_type * vec_solution = create_vector(N);

    populate_matrix(matrix_A,3,RANGE,N,N); //Pseudorandom number generation

    //Check if solution is able to converge with Jacobi method
    if(!check_if_possible(matrix_A,N))
    { 
        printf("Method is not aplicable for this seed\n\n"); //Aii != 0  , Aii > sum(rest_of_line)
        return 0;
    }

    //Pseudorandom number generation
    populate_vector(vec_B,5,RANGE,N); 

    /*Presenting initial values*/
    printf("Matrix: \n");
    print_matrix(matrix_A,N,N);

    printf("Target Vector: \n");
    print_vector(vec_B,N);

    //Starting guess, current 0, 0, 0... 0
    initial_approximation(vec_solution,N); 


    printf("Solution (Current - Difference): \n");
    
    //Starting the jacobi method
    int iteration_counter = 0;

    //Iterating while the tolerance is not reached
    while(iterate(matrix_A,vec_B,vec_solution, N, TOLERANCE)) //while there is substantial improvement (> tolerance)
        iteration_counter += 1;
    
    /* Presenting the results */
    printf("\nSolution: (%d iterations)\n",iteration_counter);
    print_vector(vec_solution,N); //Final solution

    printf("Diference from target: \n"); //Diference between Ax and b where (Ax = b)
    verify_method(matrix_A,vec_B,vec_solution, N);

    //Free memory
    delete_matrix(matrix_A,N,N); 
    delete_vector(vec_B);
    delete_vector(vec_solution);
}