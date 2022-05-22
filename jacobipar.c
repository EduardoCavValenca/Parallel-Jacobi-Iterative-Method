#include "jacobipar.h"


bool iterate_par(v_type** matrix_A, v_type* vec_B, v_type* vec_solution, int length, v_type tolerance){
    int i,j;

    v_type sum[length]; //Get line sum of Ax excluding Aii

    //For each line
    //#pragma omp parallel for default(shared) firstprivate(soma) num_threads(T)
    for(i=0; i < length; i++){ 
        sum[i] = 0;

        //For each collum
        for(j = 0; j < length; j++)
        {
            //If the element is not on the diagonal, add it to the total sum
            if(i != j)
                sum[i] += matrix_A[i][j]*vec_solution[j];
        }
    }

    //Diference between value of past and new generation, stop condition
    v_type change[length];
    v_type new_value;
    

    
    //Applying the Jacobi method
    for(i= 0 ; i < length ; i++)
    {
        new_value = (vec_B[i] - sum[i])/matrix_A[i][i]; //Jacobi Equation
        change[i] = fabs(vec_solution[i] - new_value);
        vec_solution[i] = new_value; //updates value
    }

    for(i= 0 ; i < length ; i++)
        printf("%lf-%lf   ",vec_solution[i],change[i]);

    printf("\n");

    /* ----------- Verifying the stop condition ---------------- */
    
    v_type max = FLT_MIN; //Maximum change of this iteration

    for(i= 0 ; i < length ; i++) 
        if (change[i] > max) 
            max = change[i];

    if (max < tolerance) //Very low difference, stop iterating
        return false;
    else
        return true;
            
}