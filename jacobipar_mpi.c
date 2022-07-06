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
#include <mpi.h>

#define RANGE 10
#define TOLERANCE 1e-7
#define LOG 0
#define LOG_MAX 10
#define SEED 10

void populate_matrix(double** matrix, int range, int rows, int cols);
void recalculate_matrix(double** matrix, int range, int rows, int cols, double *vec_B);
void populate_vector(double* vector, int range, int N);
double** create_matrix(int rows, int cols);
void delete_matrix(double** matrix, int rows, int cols);
void print_matrix(double** matrix, int rows, int cols);
double* create_vector(int N);
void delete_vector(double* vector);
void print_vector(double* vector, int N);
void initial_approximation(double* vec_solution, int N);
void verify_method(double** matrix_A, double* vec_B, double* vec_solution, int N, int line);

void send_lines(double** matrix, int N, int msgtag, int numprocs);
void send_vec(double* vector, int N, int msgtag, int numprocs);

//Struct of variables to count the time
typedef struct {
    double start_total;
    double end_total;

    double start_matrix;
    double end_matrix;

    double start_iteration;
    double end_iteration;

}time;


int main (int argc, char *argv[]) {

    int numprocs, rank, namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int provided;

    MPI_Status status;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

    int *bufrecv, *bufsend;

    int src, dest;
    int msgtag;

    //Rank 0 variables
    int iteration_counter;
    int send_div, send_dim;
    int main_stop_condition;

    //Other rank variables
    int dimension;
    double max_diff; //Max diff between x(k) and x(k+1)
    double max_vec; //Max value of x_i
    double stop_condition = DBL_MAX; //Tolerance
    int i,j;
    double soma;
    double* sum; //Get line sum of Ax excluding Aii
    double* change; //Diference between value of past and new generation, stop condition
    double new_value; //New Xi value
    int distance_from_zero;


    time times; //Struct to save time taken at each step
    times.start_total = omp_get_wtime(); //Start Timer of total runtime

    //Check if input is correct

    // if(argv != 3)
    // {
    //     printf("Starting values are wrong");
    //     exit(0);
    // }
    
    int N, T;
    // N = atoi(argc[1]); //Size of matrix
    // T = atoi(argc[2]); //Number of threads

    N = 10;
    T = 2;
   
    //Creating the matrix and vectors
    double ** matrix_A = create_matrix(N,N);
    double * vec_B = create_vector(N);
    double * vec_solution = create_vector(N);


    if (rank == 0) {

        matrix_A = create_matrix(N,N);
        vec_B = create_vector(N);
        vec_solution = create_vector(N);

        srand(SEED); //Define seed

        //Pseudorandom number generation for vector B
        populate_vector(vec_B, RANGE, N);

        times.start_matrix = omp_get_wtime();
        populate_matrix(matrix_A, RANGE, N, N); //Pseudorandom number generation for matrix A
        times.end_matrix = omp_get_wtime();

        //Divide by matrix_A and vec_B by main diagonal elements, and zero them
        recalculate_matrix(matrix_A, RANGE, N, N, vec_B); 

        //Starting guess, current 0, 0, 0... 0
        initial_approximation(vec_solution,N); 

        /*Presenting initial values*/
        if (LOG && N <= LOG_MAX){
            printf("Matrix: \n");
            print_matrix(matrix_A,N,N);

            printf("Target Vector: \n");
            print_vector(vec_B,N);
        }

        if(LOG && N <= LOG_MAX)
        printf("Solution (Current - Difference): \n");
    
        //Starting the Jacobi method
        times.start_iteration = omp_get_wtime();


        dimension = (N / (numprocs)); //Defini dimension of process 0
        //Send dimensions to other process
        msgtag = 0;
        send_lines(matrix_A, N, msgtag, numprocs);

        msgtag = 1;
        send_vec(vec_B, N, msgtag, numprocs);

        //send_vec(vec_solution, N, msgtag, numprocs);
        MPI_Bcast(vec_solution, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        sum = create_vector(dimension);
        change = create_vector(dimension);

        int counts[numprocs];
        int displacements[numprocs];

        for (i = 0; i < numprocs; i++){
            if (i != numprocs - 1){
                counts[i] = (N / (numprocs)); //2
                displacements[i] = counts[i] * i;
            }
            else{
                counts[i] = N - ((N / (numprocs)) * (numprocs - 1));
                displacements[i] = (N / (numprocs)) * ((numprocs - 1));
            }
        }

        distance_from_zero = (N / (numprocs)) * rank;

        MPI_Allgatherv(vec_solution, dimension, MPI_DOUBLE, vec_solution, counts, displacements, MPI_DOUBLE, MPI_COMM_WORLD); 

        print_vector(vec_solution,N);

        //Iterating        
        //for(iteration_counter = 0; stop_condition > TOLERANCE; iteration_counter++){
        for(iteration_counter = 0; iteration_counter < 1; iteration_counter++){

            //For each line
            #pragma omp parallel for private(soma,j) num_threads(T) //schedule(dynamic,1)
            for(i=0; i < dimension; i++){ 
                
                soma = 0;
                //For each collum
                for(j = 0; j < N; j++)
                        soma += matrix_A[i][j]*vec_solution[j]; //Sum of line
                sum[i] = soma;
            }

            //Applying the Jacobi method
            #pragma omp parallel for if (N > 1000) private(new_value) num_threads(T)
            for(i= 0 ; i < dimension ; i++){
                new_value = (vec_B[i] - sum[i]); //Jacobi Equation
                change[i] = fabs(vec_solution[distance_from_zero + i] - new_value);
                vec_solution[distance_from_zero + i] = new_value; //Updates value
            }

            // ----------- Verifying the stop condition ---------------- //
            
            max_diff = 0; //Maximum change of this iteration
            max_vec = 0; //Maximum value of vector of Xi 

            //Get maxs values to check stop condition
            #pragma omp parallel for reduction(max : max_diff) reduction(max : max_vec) num_threads(T)
            for(i= 0 ; i < dimension ; i++){
                max_diff = fmax(max_diff, fabs(change[i]));
                max_vec = fmax(max_vec, fabs(vec_solution[distance_from_zero + i]));
            }

            stop_condition = max_diff/max_vec; //Stop condition

            MPI_Allreduce(&stop_condition, &stop_condition, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

            MPI_Allgatherv(vec_solution, dimension, MPI_DOUBLE, vec_solution, counts, displacements, MPI_DOUBLE, MPI_COMM_WORLD); 

            print_vector(vec_solution,N); 
        }


        print_vector(vec_solution,N);

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


        int search_line = 0;
        while(search_line < 1 || search_line > N){
            printf("\nDigite a equacao desejada 1 - N: ");
            fflush(0);
            scanf("%d", &search_line);
        }
        verify_method(matrix_A,vec_B,vec_solution,N,search_line-1);



        
    }

    else {
        
        src = 0; //Main process
        dest = 0;

        if (rank != numprocs - 1)
            dimension = (N / (numprocs)); //2
        else
            dimension = N - ((N / (numprocs)) * (numprocs - 1));

        matrix_A = create_matrix(dimension,N);
        vec_B = create_vector(dimension);
        vec_solution = create_vector(N);

        //Receive matrix lines
        msgtag = 0;
        for (i = 0; i < dimension; i++)
            MPI_Recv(matrix_A[i], N, MPI_DOUBLE, src, msgtag, MPI_COMM_WORLD, &status);

        //Receive vecB values
        msgtag = 1;
        MPI_Recv(vec_B, dimension, MPI_DOUBLE, src, msgtag, MPI_COMM_WORLD, &status);

        //Receiving vec_solution
        MPI_Bcast(vec_solution, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        //Alocate variables do iterate
        sum = create_vector(dimension);
        change = create_vector(dimension);

        int counts[numprocs];
        int displacements[numprocs];

        for (i = 0; i < numprocs; i++){
            if (i != numprocs - 1){
                counts[i] = (N / (numprocs)); //2
                displacements[i] = counts[i] * i;
            }
            else{
                counts[i] = N - ((N / (numprocs)) * (numprocs - 1));
                displacements[i] = (N / (numprocs)) * ((numprocs - 1));
            }
        }

        distance_from_zero = (N / (numprocs)) * rank;

        if(rank == 1){
            vec_solution[0] = rank;
            vec_solution[1] = rank;
        } 
        if(rank == 2){
            vec_solution[0] = rank;
            vec_solution[1] = rank;
        } 
        if(rank == 3){
            vec_solution[0] = rank;
            vec_solution[1] = rank;
            vec_solution[2] = rank;
            vec_solution[3] = rank;
        }
      

        MPI_Allgatherv(vec_solution, dimension, MPI_DOUBLE, vec_solution, counts, displacements, MPI_DOUBLE, MPI_COMM_WORLD); 

        //Iterating        
        //for(iteration_counter = 0; stop_condition > TOLERANCE; iteration_counter++){
        for(iteration_counter = 0; iteration_counter < 1; iteration_counter++){

            //For each line
            #pragma omp parallel for private(soma,j) num_threads(T) //schedule(dynamic,1)
            for(i=0; i < dimension; i++){ 
                
                soma = 0;
                //For each collum
                for(j = 0; j < N; j++)
                        soma += matrix_A[i][j]*vec_solution[j]; //Sum of line
                sum[i] = soma;
            }
            
            //Applying the Jacobi method
            #pragma omp parallel for if(N>1000) private(new_value) num_threads(T)
            for(i= 0 ; i < dimension ; i++){
                new_value = (vec_B[i] - sum[i]); //Jacobi Equation
                change[i] = fabs(vec_solution[distance_from_zero + i] - new_value);
                vec_solution[distance_from_zero + i] = new_value; //Updates value
            }

            // ----------- Verifying the stop condition ---------------- //
            
            max_diff = 0; //Maximum change of this iteration
            max_vec = 0; //Maximum value of vector of Xi 

            //Get maxs values to check stop condition
            #pragma omp parallel for reduction(max : max_diff) reduction(max : max_vec) num_threads(T)
            for(i = 0 ; i < dimension ; i++){
                max_diff = fmax(max_diff, fabs(change[i]));
                max_vec = fmax(max_vec, fabs(vec_solution[distance_from_zero + i]));
            }

            stop_condition = max_diff/max_vec; //Stop condition

            MPI_Allreduce(&stop_condition, &stop_condition, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

            print_vector(vec_solution,N);
            MPI_Allgatherv(vec_solution, dimension, MPI_DOUBLE, vec_solution, counts, displacements, MPI_DOUBLE, MPI_COMM_WORLD); 
            

        }
        
      
    }

	MPI_Finalize();
	return 0;
 
}


void send_vec(double* vector, int N, int msgtag, int numprocs){

    int send_line;
    int division;
    int i, dest;

    for (dest = 1; dest < numprocs; dest++){
            if (dest != numprocs - 1)
                division = (N / (numprocs));
            else
                division = N - ((N / (numprocs)) * (numprocs - 1));

            // for(i = 0; i < division; i++){
            //     send_line = ((dest) * (N / (numprocs - 1))) + i;
                MPI_Send(vector + (dest * (N / (numprocs))), division , MPI_DOUBLE, dest, msgtag, MPI_COMM_WORLD);
            //}
    }
}


void send_lines(double** matrix, int N, int msgtag, int numprocs){

    int send_line;
    int division;
    int i, dest;

    for (dest = 1; dest < numprocs; dest++){
            if (dest != numprocs - 1)
                division = (N / (numprocs));
            else
                division = N - ((N / (numprocs)) * (numprocs - 1));

            for(i = 0; i < division; i++){
                send_line = ((dest) * (N / (numprocs))) + i;
                MPI_Send(matrix[send_line], N , MPI_DOUBLE, dest, msgtag, MPI_COMM_WORLD);
            }
    }
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
        vector[i] = randomNumber(); //Random doubles numbers

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
    //Free matrix memory in heap
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
