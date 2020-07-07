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

    //--------orginial-----------//
    if(len <= 1000){
        start = wtime();
        if (mat_mul(a, b, c, len) != 0){
            fprintf(stderr, "Failed to Matrix mul\n");
            exit(1);
        }
        stop = wtime();
        // print_matrix(a,"A", len);
        // print_matrix(b,"B", len);
        // print_matrix(c,"C", len);
        printf("1.Processing time: %f\n", stop-start);

        //initialize//
        for(int i=0; i<len; i++){
            for(int j=0; j<len; j++){
                c[i][j]=0;
            }
        }
    }
    else{
        printf("1.Skip original mat_mul function\n");
    }

    
    //--------1 row per thread -----------//
    start = wtime();
    if (mat_mul_th(a, b, c, len) != 0){
        fprintf(stderr, "Failed to Matrix mul\n");
        exit(1);
    }
    stop = wtime();
    // print_matrix(a,"A", len);
    // print_matrix(b,"B", len);
    // print_matrix(c,"C", len);
    printf("2.Processing time (1 row per thread): %f\n", stop-start);

    //initialize//
    for(int i=0; i<len; i++){
        for(int j=0; j<len; j++){
            c[i][j]=0;
        }
    }
    

    //--------len/core row per thread -----------//
    if(len <8){
        num_thread = len;
    }
    start = wtime();
    if (mat_mul_th_core(a, b, c, len, num_thread) != 0){
        fprintf(stderr, "Failed to Matrix mul\n");
        exit(1);
    }
    stop = wtime();

    // // print_matrix(a,"A", len);
    // // print_matrix(b,"B", len);
    // print_matrix(c,"C", len);
    printf("3.Processing time(%d row per thread): %f\n",len/num_thread, stop-start);

    return 0;
}

