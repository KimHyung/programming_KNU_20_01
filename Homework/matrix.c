#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void matinit(int ***p_a, int ***p_b, int ***p_c, int len)
{
    int **a;
    int **b;
    int **c;
    int i,j;

    a = (int**)malloc(len * sizeof(int*));
    b = (int**)malloc(len * sizeof(int*));
    c = (int**)malloc(len * sizeof(int*));

    for(i=0;i<len;i++){
        a[i] =(int*)malloc(len * sizeof(int));
        b[i] =(int*)malloc(len * sizeof(int));
        c[i] =(int*)malloc(len * sizeof(int));
    }
    
    srandom((unsigned int )time(NULL));
    for(i=0;i<len;i++){ //i(Row)
        for(j=0;j<len;j++){ //j(Cloumn)
           a[i][j] = random() % 10;
           b[i][j] = random() % 10;
           c[i][j] = 0;
        }
    }

    *p_a = a;
    *p_b = b;
    *p_c = c;
}

void print_matrix(int** matrix, char* name, int len)
{
    printf("=== %s mtx==\n",name);
    for(int i=0;i<len;i++){ //i(Row)
        for(int j=0;j<len;j++){ //j(Cloumn)
            printf("%d ",matrix[i][j]);
        }
        printf("\n");
    }
}

double wtime()
{
    static sec = -1;
    struct timeval tv;   
    gettimeofday(&tv, NULL);
    if(sec<0)
        sec = tv.tv_sec;
    return (tv.tv_sec - sec) + 1.0e-68*tv.tv_usec;
}