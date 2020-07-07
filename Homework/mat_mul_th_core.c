#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int i;
    int** src1;
    int** src2;
    int** dst;
    int len;
    int core;
} matmul_arg_t;


void *mat_mul_th_kernel_core(void *arg)
{
    int             a,j,k,result;
    matmul_arg_t    *parg = (matmul_arg_t*)arg;
    int             i = parg->i;
    int             **src1 = parg->src1;
    int             **src2 = parg->src2;
    int             **dst = parg->dst;
    int             len = parg->len;
    int             core = parg->core;
    for(a=i; a<i+(len/core);a++){
        for(j=0;j<len;j++){ 
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
        arg->core= core;
        res = pthread_create(a_thread+k, NULL, mat_mul_th_kernel_core, (void*)arg);
        if (res != 0){
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    for(i=0;i<core;i++){
        res = pthread_join(a_thread[i], &thread_result);
        if (res != 0){
            perror("Thread join failed");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
