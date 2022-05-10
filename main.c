#include "matrix.h"
#include "vector.h"


int main(){
    
    int N;
    scanf("%d", &N);

    int ** matrix_A = create_matrix(N,N);
    int * vec_B = create_vector(N);

    populate_matrix(matrix_A,1,100,N,N);
    populate_vector(vec_B,2,100,N);
    
    print_matrix(matrix_A,N,N);
    print_vector(vec_B,N);

    delete_matrix(matrix_A,N,N);
    delete_vector(vec_B);
}