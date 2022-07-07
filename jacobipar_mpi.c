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

//MPI functions
void send_lines(double** matrix, int N, int msgtag, int numprocs);
void send_vec(double* vector, int N, int msgtag, int numprocs);
void set_Allgatherv_parameters (int* counts, int* displacements, int N ,int numprocs);

//Jacobi fuctions
void populate_matrix(double** matrix, int range, int rows, int cols);
void recalculate_matrix(double** matrix, int range, int rows, int cols, double *vec_B);
void populate_vector(double* vector, int range, int N);
void initial_approximation(double* vec_solution, int N);
void verify_method(double** matrix_A, double* vec_B, double* vec_solution, int N, int line);

//Help fuctions
double** create_matrix(int rows, int cols);
double* create_vector(int N);
int* create_int_vector(int N);
void delete_matrix(double** matrix, int rows, int cols);
void delete_vector(double* vector);
void delete_int_vector(int* vector);
void print_matrix(double** matrix, int rows, int cols);
void print_vector(double* vector, int N);


//Struct of variables to count the time
typedef struct {

    double start_iteration;
    double end_iteration;

}time;


int main (int argc, char *argv[]) {

    //Main MPI variables
    int numprocs, rank, namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int provided;

    //Main MPI variables atribution
    MPI_Status status;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

    //MPI needed variables
    int src, dest;
    int msgtag;
    int* counts;
    int* displacements;

    //Creating the matrix and vectors pointers
    double ** matrix_A;
    double * vec_B;
    double * vec_solution;

    //Main variables
    int iteration_counter;  //Total iterations
    int dimension;          //Number of lines of certain process
    int distance_from_zero; //vector right index for certain process
    int N;                  //Matrix size
    int T;                  //Number of threads
    int search_line;        //Line for method evaluation 

    //Iteration variables
    double max_diff;        //Max diff between x(k) and x(k+1)
    double max_vec;         //Max value of x_i
    double stop_condition = DBL_MAX; //Tolerance
    
    double local_sum;       //Local sum variable
    double* sum;            //Get line sum of Ax excluding Aii
    double* change;         //Diference between value of past and new generation, stop condition
    double new_value;       //New Xi value

    //Help variables
    int i,j;
    time times;             //Struct to save time taken at each step
    
    //Check if input is correct

    // if(argv != 3)
    // {
    //     printf("Starting values are wrong");
    //     exit(0);
    // }
    
    // N = atoi(argc[1]); //Size of matrix
    // T = atoi(argc[2]); //Number of threads

    N = 10000;
    T = 2;
   

    if (rank == 0) {

        srand(SEED); //Define seed

        //Alocate the matrix and vectors memory
        matrix_A = create_matrix(N,N);
        vec_B = create_vector(N);
        vec_solution = create_vector(N);

        //Pseudorandom number generation for vector B
        populate_vector(vec_B, RANGE, N);

        populate_matrix(matrix_A, RANGE, N, N); //Pseudorandom number generation for matrix A

        //Divide by matrix_A and vec_B by main diagonal elements, and zero them
        recalculate_matrix(matrix_A, RANGE, N, N, vec_B); 

        //Starting guess, current 0, 0, 0... 0
        initial_approximation(vec_solution,N); 
    
        //Process variables
        dimension = (N / (numprocs)); //Number of lines of certain process
        distance_from_zero = (N / (numprocs)) * rank; //vector right index for certain process

        //Send matrix lines to other process
        msgtag = 0;
        send_lines(matrix_A, N, msgtag, numprocs);

        //Send vec_B values to other process
        msgtag = 1;
        send_vec(vec_B, N, msgtag, numprocs);

        //Broadcast of vec_solution
        MPI_Bcast(vec_solution, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        //Distribution variables for allgatherv use
        counts = create_int_vector(numprocs);
        displacements = create_int_vector(numprocs);
        set_Allgatherv_parameters(counts, displacements, N, numprocs);

        //Vectors to save current iteration results
        sum = create_vector(dimension);
        change = create_vector(dimension);        

        //Starting the Jacobi method
        times.start_iteration = omp_get_wtime();

        //Iterating        
        for(iteration_counter = 0; stop_condition > TOLERANCE; iteration_counter++){

            //For each line
            #pragma omp parallel for private(local_sum,j) num_threads(T) //schedule(dynamic,1)
            for(i=0; i < dimension; i++){ 
                
                local_sum = 0;
                //For each collum
                for(j = 0; j < N; j++)
                        local_sum += matrix_A[i][j]*vec_solution[j]; //Sum of line
                sum[i] = local_sum;
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

            //Get max stop_condition of all process
            MPI_Allreduce(&stop_condition, &stop_condition, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

            //Update vec_solution with values of all process
            MPI_Allgatherv(&vec_solution[distance_from_zero], dimension, MPI_DOUBLE, vec_solution, counts, displacements, MPI_DOUBLE, MPI_COMM_WORLD); 

        }

        //####################### End of Iterations #############################
        times.end_iteration = omp_get_wtime();

        printf("Number of Iterations: %d\n\n", iteration_counter);
        printf("Time taken iterating: %lf\n",  times.end_iteration - times.start_iteration);

        search_line = 0;
        while(search_line < 1 || search_line > N){
            printf("\nDigite a equacao desejada 1 - N: ");
            fflush(0);
            scanf("%d", &search_line);
        }

        verify_method(matrix_A,vec_B,vec_solution,N,search_line-1);

        //Free memory
        delete_matrix(matrix_A, N,N);
        delete_vector(vec_B);
        delete_vector(vec_solution);
        delete_vector(sum);
        delete_vector(change);
        delete_int_vector(counts);
        delete_int_vector(displacements);
    
    }

    else {
        
        //Main process
        src = 0; 
        dest = 0;

        //Number of lines of certain process
        if (rank != numprocs - 1)
            dimension = (N / (numprocs)); //2
        else
            dimension = N - ((N / (numprocs)) * (numprocs - 1));

        //vector right index for certain process
        distance_from_zero = (N / (numprocs)) * rank; 

        //Alocate the matrix and vectors memory
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

        //Distribution variables for allgatherv use
        counts = create_int_vector(numprocs);
        displacements = create_int_vector(numprocs);
        set_Allgatherv_parameters(counts, displacements, N, numprocs);

        //Vectors to save current iteration results
        sum = create_vector(dimension);
        change = create_vector(dimension);        

        //Iterating        
        for(iteration_counter = 0; stop_condition > TOLERANCE; iteration_counter++){

            //For each line
            #pragma omp parallel for private(local_sum,j) num_threads(T) //schedule(dynamic,1)
            for(i=0; i < dimension; i++){ 
                
                local_sum = 0;
                //For each collum
                for(j = 0; j < N; j++)
                        local_sum += matrix_A[i][j]*vec_solution[j]; //Sum of line
                sum[i] = local_sum;
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

            //Get max stop_condition of all process
            MPI_Allreduce(&stop_condition, &stop_condition, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

            //Update vec_solution with values of all process
            MPI_Allgatherv(&vec_solution[distance_from_zero], dimension, MPI_DOUBLE, vec_solution, counts, displacements, MPI_DOUBLE, MPI_COMM_WORLD); 
        }

        //Free memory
        delete_matrix(matrix_A, dimension,N);
        delete_vector(vec_B);
        delete_vector(vec_solution);
        delete_vector(sum);
        delete_vector(change);
        delete_int_vector(counts);
        delete_int_vector(displacements);
      
    }


	MPI_Finalize();
	return 0;
 
}



//Distribution variables for allgatherv use
void set_Allgatherv_parameters (int* counts, int* displacements, int N ,int numprocs){

    int i;

    for (i = 0; i < numprocs; i++){
            if (i != numprocs - 1){
                counts[i] = (N / (numprocs)); 
                displacements[i] = counts[i] * i;
            }
            else{
                counts[i] = N - ((N / (numprocs)) * (numprocs - 1));
                displacements[i] = (N / (numprocs)) * ((numprocs - 1));
            }
    }
}


//Send vector parts for each process
void send_vec(double* vector, int N, int msgtag, int numprocs){

    int send_line;
    int division;
    int i, dest;

    for (dest = 1; dest < numprocs; dest++){
            if (dest != numprocs - 1)
                division = (N / (numprocs));
            else
                division = N - ((N / (numprocs)) * (numprocs - 1));

            MPI_Send(vector + (dest * (N / (numprocs))), division , MPI_DOUBLE, dest, msgtag, MPI_COMM_WORLD);
            
    }
}

//send matrix lines for each process
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

int* create_int_vector(int N){
    //Allocate vector in heap
    
    int* vector;
    vector = (int *)malloc(N * sizeof(int));   

    return vector;

}

void delete_int_vector(int* vector){
    //free vector memory in heap
    free(vector);   
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
