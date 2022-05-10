#include "matrix.h"

int main(){
    
    int N;
    scanf("%d", &N);

    int ** matrix = create_matrix(N,N);
    populate_matrix(matrix,1,100,N,N);
    print_matrix(matrix,N,N);
    delete_matrix(matrix,N,N);
}