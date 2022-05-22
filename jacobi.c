#include "jacobi.h"

void initial_approximation(v_type* vec_solution, int length){
    //Starting guess, current 0, 0, 0... 0 
    int i;

    //Add zeros the vec_solutions
    for (i= 0 ; i < length ; i++)
        vec_solution[i] = 0;
}


void verify_method(v_type** matrix_A, v_type* vec_B, v_type* vec_solution, int length){
    //Verify difference between expected and founded value.

    int i, j;

    v_type diff, sum;

    for(i=0; i< length; i++)
    {
        sum = 0;
        for(j=0; j< length; j++){
            sum += matrix_A[i][j]*vec_solution[j];
        }
        diff = fabs(sum - vec_B[i]);
        printf("%lf ", diff);
    }
    printf("\n\n");

}

bool check_if_possible(v_type** matrix_A, int length){
    //Check if solution is able to converge with Jacobi method

    int i,j;

    v_type sum;


    for (i = 0; i < length; i++){
        sum = 0;
        for (j = 0; j < length; j++){
            if (i != j)
                sum += fabs(matrix_A[i][j]);
        }

        //If the sum of all others values is grater than the diagonal value, is invalid
        if (sum >= matrix_A[i][i] || matrix_A[i][i] == 0) //Aii != 0  , Aii > sum(rest_of_line)
            return false;
    }

    return true;
}