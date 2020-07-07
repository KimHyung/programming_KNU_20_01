#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define LOOP_COUNTER 100000

void *thread_function1(void *arg);
void *thread_function2(void *arg);

int counter;

int main()
{
    int res1, res2;
    pthread_t a_thread1, a_thread2;
    void *thread_result;

    res1 = pthread_create(&a_thread1, NULL, thread_function1, NULL);
    res2 = pthread_create(&a_thread2, NULL, thread_function2, NULL);

    if(res1 != 0){
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    if(res2 !=0){
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    res1 = pthread_join(a_thread1, &thread_result);
    res2 = pthread_join(a_thread2, &thread_result);

    printf("main : Final coutner: %d\n", counter);

    exit(EXIT_SUCCESS);
}

void *thread_function1(void *arg)
{
    int i =0;

    for(i=0; i<LOOP_COUNTER; i++){
        counter = counter+1;
        //printf("Thread1: Counter = %d\n", counter);
    }
    pthread_exit("Thank you for the CPU time");
}

void *thread_function2(void *arg)
{
    int i =0;

    for(i=0; i<LOOP_COUNTER; i++){
        counter = counter-1;
        //printf("Thread2: Counter = %d\n", counter);
    }
    pthread_exit("Thank you for the CPU time");
}