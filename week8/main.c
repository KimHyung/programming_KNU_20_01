#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "matrix.h"

double wtime();

int main(int argc, char** argv)
{
    int **a;
    int **b;
    int **c;
    int i,j,k;
    int len, num_thread;
    double start, stop;

    if (argc == 2){
        len = atoi(argv[1]);
        num_thread = 1;
    }
    else if (argc == 3){
        len = atoi(argv[1]);    
        num_thread = atoi(argv[2]);
    }
    else{
        printf("Usage: matrix [row] [num thread] \n");
        exit(1);
    }

    matinit(&a, &b, &c, len);

    
    // if(mat_add(a, b, c, len) !=0){
    //     fprintf(stderr, "Failed to Matrix add\n");
    //     exit(1);
    // }
    // if(mat_sub(a, b, c, len) !=0){
    //     fprintf(stderr, "Failed to Matrix sub\n");
    //     exit(1);
    // }
    // start = wtime();
    // if (mat_mul(a, b, c, len) != 0){
    //     fprintf(stderr, "Failed to Matrix mul\n");
    //     exit(1);
    // }
    // stop = wtime();
    // if (mat_inv(b, c, len) != 0){
    //     fprintf(stderr, "Inverse matrix isn't existed.");
    //     exit(1);
    // }
    

    // print_matrix(a,"A", len);
    // print_matrix(b,"B", len);
    // print_matrix(c,"C", len);
    // printf("Processing time: %f\n", stop-start);

    start = wtime();
    if (mat_mul_th(a, b, c, len) != 0){
        fprintf(stderr, "Failed to Matrix mul\n");
        exit(1);
    }
    stop = wtime();
    // // print_matrix(c,"C", len);
    // printf("Processing time (thread): %f\n", stop-start);
    //---------------//
    // for(int i=0; i<len; i++){
    //     for(j=0;j<len;j++){
    //         c[i][j] = 0;
    //     }
    // }
    // start = wtime();
    // if (mat_mul_th_core(a, b, c, len, num_thread) != 0){
    //     fprintf(stderr, "Failed to Matrix mul\n");
    //     exit(1);
    // }
    // stop = wtime();

    // // print_matrix(a,"A", len);
    // // print_matrix(b,"B", len);
    // // print_matrix(c,"C", len);
    printf("Processing time (8core-> 8thread): %f\n", stop-start);
    return 0;
}

