#include "matrix.h"
#include "vector.h"
#include "jacobi.h"

#define RANGE 1
#define TOLERANCE 0.0001

int main(){
    
    int N;
    scanf("%d", &N);

    v_type ** matrix_A = create_matrix(N,N);
    v_type * vec_B = create_vector(N);
    v_type * vec_solution = create_vector(N);

    populate_matrix(matrix_A,3,RANGE,N,N);

    if(!check_if_possible(matrix_A,N)){
        printf("Method is not aplicable for this seed\n\n");
        return 0;
    }

    populate_vector(vec_B,5,RANGE,N);

    printf("Matrix: \n");
    print_matrix(matrix_A,N,N);

    printf("Target Vector: \n");
    print_vector(vec_B,N);


    initial_approximation(vec_solution,N);

    printf("Solution (Current - Difference): \n");
    
    int iteration_counter = 0;
    while(iterate(matrix_A,vec_B,vec_solution, N, TOLERANCE))
        iteration_counter += 1;
    
    printf("\nSolution: (%d iterations)\n",iteration_counter);
    print_vector(vec_solution,N);

    printf("Diference from target: \n");
    verify_method(matrix_A,vec_B,vec_solution, N);

    delete_matrix(matrix_A,N,N);
    delete_vector(vec_B);
}