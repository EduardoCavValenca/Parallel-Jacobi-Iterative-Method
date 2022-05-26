#include "jacobipar.h"


bool iterate_par(v_type** matrix_A, v_type* vec_B, v_type* vec_solution, int length, v_type tolerance, int LOG){
    int i,j = 0;

    v_type sum[length]; //Get line sum of Ax excluding Aii

    v_type soma = 0;
    //For each line
    #pragma omp parallel for firstprivate(soma,j) num_threads(T) schedule(dynamic,1)
    for(i=0; i < length; i++){ 
        
        soma = 0;
        //For each collum
        for(j = 0; j < length; j++)
        {
            //If the element is not on the diagonal, add it to the total sum
            if(i != j)
                soma += matrix_A[i][j]*vec_solution[j];
        }

        sum[i] = soma;
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

    if (LOG){
        for(i= 0 ; i < length ; i++)
            printf("%lf-%lf   ",vec_solution[i],change[i]);

        printf("\n");
    }

    /* ----------- Verifying the stop condition ---------------- */
    
    v_type max_dif = 0; //Maximum change of this iteration

    #pragma omp parallel for reduction(max : max_dif)
    for(i= 0 ; i < length ; i++) 
        max_dif = fmax(max_dif, change[i]);

    if (max_dif < tolerance) //Very low difference, stop iterating
        return false;
    else
        return true;
            
}