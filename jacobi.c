#include "jacobi.h"

void initial_approximation(v_type* vec_solution, int length){
    //Starting guess, current 0, 0, 0... 0 
    int i;

    for (i= 0 ; i < length ; i++)
        vec_solution[i] = 0;
}


bool iterate(v_type** matrix_A, v_type* vec_B, v_type* vec_solution, int length, v_type tolerance){
    int i,j;

    v_type sum[length]; //Get line sum of Ax excluding Aii

    for(i=0; i < length; i++){ 
        sum[i] = 0;
        for(j = 0; j < length; j++){
            if(i != j)
                sum[i] += matrix_A[i][j]*vec_solution[j];
        }
    }

    v_type change[length]; //Diference between value of past and new generation, stop condition
    v_type new_value;

    for(i= 0 ; i < length ; i++){
        new_value = (vec_B[i] - sum[i])/matrix_A[i][i]; //Jacobi Method
        change[i] = fabs(vec_solution[i] - new_value);
        vec_solution[i] = new_value; //updates value
    }

    for(i= 0 ; i < length ; i++)
        printf("%f-%f   ",vec_solution[i],change[i]);

    printf("\n");

    v_type max = FLT_MIN; //Maximum change of this iteration

    for(i= 0 ; i < length ; i++) 
        if (change[i] > max) 
            max = change[i];

    if (max < tolerance) //Very low difference, stop iterating
        return false;
    else
        return true;
            
}


void verify_method(v_type** matrix_A, v_type* vec_B, v_type* vec_solution, int length){
    //Verify difference between expected and founded value.

    int i, j;

    v_type diff, sum;

    for(i=0; i< length; i++){
        sum = 0;
        for(j=0; j< length; j++){
            sum += matrix_A[i][j]*vec_solution[j];
        }
        diff = fabs(sum - vec_B[i]);
        printf("%f ", diff);
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
        if (sum >= matrix_A[i][i] || matrix_A[i][i] == 0) //Aii != 0  , Aii > sum(rest_of_line)
            return false;
    }

    return true;
}