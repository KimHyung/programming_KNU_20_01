#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
 

typedef int buffer_item;
#define BUFFER_SIZE 5

typedef struct {
int buffer[BUFFER_SIZE];
int full;
int empty;
pthread_mutex_t *mutex;
} queue;
 
void *consumer(void *param);
void *producer(void *param);
int insert_item(buffer_item item,void *param);
int remove_item(buffer_item *item,void *param);
int getRandom(int min, int max);

int producer_sleep_time_max;
int consumer_sleep_time_max;
int start = 1;

 

int main(int argc, char *argv[])
{
    int main_sleep_time;
    // if user didn’t enter how long to sleep, number of producer and consumer, error
    if(argc != 4){
        printf("Program requires 3 integer arguments" );
        return -1;
    }
    else if( atoi(argv[2]) < 0 || atoi(argv[3]) < 0 || atoi(argv[1]) < 0){
        printf("Program requires 3 positive integers for: Main sleep time, Producer sleep time and Consumer sleep time");
        return -1;
    }
    else{
        //Get the main sleep time, producer sleep time max and consumer sleep time max
        main_sleep_time = atoi(argv[1]);
        producer_sleep_time_max = atoi(argv[2]);
        consumer_sleep_time_max = atoi(argv[3]);
    }

    queue *ptr;
    ptr->full = 0;
    ptr->empty = BUFFER_SIZE;
    //initialize buffer to negatives
    ptr->buffer[0] = -1;
    ptr->buffer[1] = -1;
    ptr->buffer[2] = -1;
    ptr->buffer[3] = -1;
    ptr->buffer[4] = -1;
    pthread_t conid;
    pthread_t proid;
    //create mutex lock
    pthread_mutex_init(ptr->mutex,NULL);
    //create  producer thread
    pthread_create(&proid,NULL,producer,ptr);
    //create consumer thread
    pthread_create(&conid,NULL,consumer,ptr);
    // Send main thread to sleep
    sleep(main_sleep_time);
    //stop the threads
    pthread_cancel(proid);
    pthread_cancel(conid);
    printf("nFinal Buffer Content:");
    
    for(int i =0;i< BUFFER_SIZE;i++){
        printf("%d",ptr->buffer[i]);
        printf("t");
    }
    printf("n");
    exit(0);
}

void *producer(void *param)
{
    queue* prod = (queue*)param;
    buffer_item item;
    while(start){   
        srand(time(NULL));
        int random_sleep_time = getRandom(0, producer_sleep_time_max);
        sleep(random_sleep_time);
        //acquire mutex lock
        pthread_mutex_lock(prod->mutex);
        item = getRandom(0,1000);
        if(insert_item(item,prod) == -1){
            printf("nProduction failed");
        }
        else{
            printf("nProducer produced:%d",item);
            //print buffer content
            printf("nBuffer content:");
            for(int i=0;i < BUFFER_SIZE;i++){
                printf("%d",prod->buffer[i]);
                printf("t");
            }
        }
        //release mutex
        pthread_mutex_unlock(prod->mutex);
    }
}

void *consumer(void *param)
{
    buffer_item item;
    queue* con = (queue*)param;
    while(start){
        srand(time(NULL));
        int random_sleep_time = getRandom(0, consumer_sleep_time_max);
        sleep(random_sleep_time);
        //acquire mutex lock
        pthread_mutex_lock(con->mutex);
        if(remove_item(&item,con) == -1)
            printf("nConsumer could not consume");
        else{
            printf("nConsumer consumed:%d",item);
            //print buffer content
            printf("nBuffer content");
            for(int i=0; i < BUFFER_SIZE;i++){
                printf("%d",con->buffer[i]);
                printf("t");
            }
        }
        //Release mutex
        pthread_mutex_unlock(con->mutex);
    }
}

int getRandom(int min, int max){
    return ( rand()% ((max-min)+1));
}

int insert_item(buffer_item item,void *param){
    queue* in = (queue*)param;

    if(in->empty > 0 && in->full < BUFFER_SIZE){
        in->buffer[BUFFER_SIZE - in->empty] = item;
        in->empty--;
        in->full++;
        return 0;
    }
    else
        return -1;
}

int remove_item(buffer_item *item,void *param){
        queue* out = (queue*)param;
    if(out->full > 0){
        *item = out->buffer[out->full-1];
        out->buffer[out->full-1]=-1;
        out->empty++;
        out->full--;
        return 0;
    }
    else
        return -1;
}