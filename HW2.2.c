#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>

#define NUM_THREADS 16
#define MAX_VALUE 100000001

int rank[3]={-1,-1,-1};
pthread_mutex_t rank_mutex;

void *countChallenge(void *threadid){
    sleep(1);
    int *id_t = (int *) threadid;
    int taskid = *id_t;
    int number = 0;

    printf("Thread %d => Start Count \n",taskid);
    while(number<=MAX_VALUE){
        number++;
        if(number>MAX_VALUE){
            break;
        }
    }
    pthread_mutex_lock(&rank_mutex);
    for(int i = 0;i<3;i++){
        if(rank[i]== -1){
            rank[i] = taskid;
            break;
        }
    }
    pthread_mutex_unlock(&rank_mutex);
    pthread_exit(NULL);
}
int main(int argc,char *argv[]){
    pthread_t threads[NUM_THREADS];

    int *taskids[NUM_THREADS];
    int rc;
    void *status;

    pthread_mutex_init(&rank_mutex,NULL);

    for(int i = 0;i<NUM_THREADS;i++){
        taskids[i] =(int *) malloc(sizeof(int));
        *taskids[i] = i;
        printf(":> Stanby Threat %d \n",i);
        rc = pthread_create(&threads[i],NULL,countChallenge,(void*)taskids[i]);
        if (rc){
            printf("ERR: form create in %d \n",i);
            exit(-1);
        }
    }
    for(int t=0;t<NUM_THREADS;t++){
        rc = pthread_join(threads[t],&status);
    }
    for(int i = 0;i<3;i++){
        printf("*-------------------------------------------*\n");
        printf("Rank %d is > Thread %d => Stop Count \n",i+1,rank[i]);
    }

    pthread_mutex_destroy(&rank_mutex);

    pthread_exit(NULL);

}