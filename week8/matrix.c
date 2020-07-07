#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

float determin(int** , int );

int mat_add(int** src1, int** src2, int** dst, int len)
{
    int i,j;
    for(i=0; i<len; i++){
        for(j=0;j<len;j++){
            dst[i][j]= src1[i][j] + src2[i][j];
        }
    }
    return 0; 
}

int mat_sub(int** src1, int** src2, int** dst, int len)
{
    int i,j;
    for(i=0; i<len; i++){
        for(j=0;j<len;j++){
            dst[i][j]= src1[i][j] - src2[i][j];
        }
    }
    return 0; 
}

int mat_mul(int** src1, int** src2, int** dst, int len)
{
    int i,j,k;
    int result=0;
    for(i=0;i<len;i++){ //i(Row)
        for(j=0;j<len;j++){ //j(Cloumn)
            result = 0;
            for(k=0; k< len; k++){
                result += src1[i][k] * src2[k][j];
            }
            dst[i][j] = result;
        }
    }
    return 0;
}

typedef struct {
    int i;
    int** src1;
    int** src2;
    int** dst;
    int len
} matmul_arg_t;

// int mat_mul_th_kernel(int i, int** src1, int** src2, int** dst, int len)
void *mat_mul_th_kernel(void *arg)
{
    int             j,k,result;
    matmul_arg_t    *parg = (matmul_arg_t*)arg;
    int             i = parg->i;
    int             **src1 = parg->src1;
    int             **src2 = parg->src2;
    int             **dst = parg->dst;
    int             len = parg->len;
    for(j=0;j<len;j++){ //j(Cloumn)
        //c[i][j] = a[i][j] + b[i][j]; //add matrix
        result = 0;
        for(k=0; k< len; k++){
            result += src1[i][k] * src2[k][j];
        }
        dst[i][j] = result;
    }
}

int mat_mul_th(int** src1, int** src2, int** dst, int len)
{
    int             i,j,k,res;
    matmul_arg_t    *arg;
    pthread_t       *a_thread;
    void            *thread_result;

    a_thread = (pthread_t*)malloc(sizeof(pthread_t) * len);
    for(i=0;i<len;i++){ //i(Row)
        arg = (matmul_arg_t*)malloc(sizeof(matmul_arg_t));
        arg->i = i;
        arg->src1 = src1;
        arg->src2 = src2;
        arg->dst = dst;
        arg->len = len;
        res = pthread_create(a_thread+i, NULL, mat_mul_th_kernel, (void*)arg);
        if (res != 0){
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }
    for(i=0;i<len;i++){ //i(Row)
        res = pthread_join(a_thread[i], &thread_result);
        if (res != 0){
            perror("Thread join failed");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

void *mat_mul_th_kernel_core(void *arg)
{
    int             a,j,k,result;
    matmul_arg_t    *parg = (matmul_arg_t*)arg;
    int             i = parg->i;
    int             **src1 = parg->src1;
    int             **src2 = parg->src2;
    int             **dst = parg->dst;
    int             len = parg->len;
    for(a=i; a<i+(len/8);a++){
        for(j=0;j<len;j++){ //j(Cloumn)
            //c[i][j] = a[i][j] + b[i][j]; //add matrix
            result = 0;
            for(k=0; k< len; k++){
                result += src1[a][k] * src2[k][j];
            }
            dst[a][j] = result;
        }
    }
}

int mat_mul_th_core(int** src1, int** src2, int** dst, int len, int core)
{
    int             i,j,k,res;
    matmul_arg_t    *arg;
    pthread_t       *a_thread;
    void            *thread_result;

    a_thread = (pthread_t*)malloc(sizeof(pthread_t) * core);
    for(k=0; k<core; k++){
        arg = (matmul_arg_t*)malloc(sizeof(matmul_arg_t));
        arg->i = k*(len/core);
        arg->src1 = src1;
        arg->src2 = src2;
        arg->dst = dst;
        arg->len = len;
        // printf("test%d: i=%d, len=%d, core=%d\n",k,arg->i,len,core);
        res = pthread_create(a_thread+k, NULL, mat_mul_th_kernel_core, (void*)arg);
        if (res != 0){
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    for(i=0;i<core;i++){ //i(Row)
        res = pthread_join(a_thread[i], &thread_result);
        if (res != 0){
            perror("Thread join failed");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

// 0: Success
// 1: Inverse matrix isn't existed.
int mat_inv(int** src1, int** dst, int len)
{
    int i,j;
    float det=0;
    det = det + determin(src1, len);
    if(det == 0){
        return 1;
    }
    else{
        // cofac(src1, len);
    }

    return 0;
}


float determin(int** matrix, int len){
    float deter=0.0, z=1.0;
    int a, b, c, x, y;
    int** mt;
    
    mt = (int**)malloc(len * sizeof(int*));
    
    for(int i=0;i<len;i++){
        mt[i] =(int*)malloc(len * sizeof(int));
    }
    if(len==1)
    {
        return(matrix[0][0]);
    }
    else
    {
        deter=0;
        for(a=0;a<len;++a){
            x=0;
            y=0;
            for(b=0;b<len;++b){
                for(c=0;c<len;++c){
                    mt[b][c]=0;
                    if((b != 0) && (c != a))
                    {
                        mt[x][y]=matrix[b][c];
                        if(y<(len-2))
                            y++;
                        else
                        {
                            y=0;
                            x++;
                        }
                    }
                }
            }
            deter=deter + z * (matrix[0][a] * determin(mt,len-1));
            z=-1 * z;
        }
    }
    return(deter);
}

// void cofac(int** comatr, int len){
//     int** matr;
//     int** cofact;
//     int a, b, c, d, x, y;

//     matr = (int**)malloc(len * sizeof(int*));
//     cofact = (int**)malloc(len * sizeof(int*));

//     for(int i=0;i<len;i++){
//         matr[i] =(int*)malloc(len * sizeof(int));
//         cofact[i] =(int*)malloc(len * sizeof(int));
//     }

//     for(c=0; c<len; ++c){
//         for(d=0; d<len; ++d){
//             x=0;
//             y=0;
//             for(a=0;a<len; ++a){
//                 for(b=0; b<len; ++b){
//                     if(a != c && b != d)
//                     {
//                         matr[x][y]=comatr[a][b];
//                         if(y<(len-2))
//                             y++;
//                         else
//                         {
//                             y=0;
//                             x++;
//                         }
//                     }
//                 }           
//             }
//             cofact[c][d] = pow(-1,c + d) * determin(matr,len-1);
//         }
//     }
//     // trans(comatr, cofact, len);
// }

// void trans(float matr[40][40], float m1[40][40], int r){
//     float inv_matrix[40][40], tran[40][40], d;
//     int a,b;
//     for(a=0;a<r;++a){
//         for(b=0;b<r;++b){
//             tran[a][b]=m1[b][a];
//         }   
//     }
//     d=determin(matr, r);
//     for(a=0;a<r;++a){
//         for(b=0;b<r;++b){
//             inv_matrix[a][b]=tran[a][b] / d;
//         }
//     }
//     printf("\n\n\n The Inverse of matrix is . . . \n\n");
//     for(a=0;a<r;++a){
//     for(b=0;b<r;++b)
//     printf("\t%f", inv_matrix[a][b]);
//     printf("\n\n");
//     }
// }

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