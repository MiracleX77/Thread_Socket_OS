#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREADS 4

char name[] = {"Phatchara Rakchuai"};
void *doSometing(void *threadid){
    long t_id = (long) threadid;
    switch (t_id)
    {
    case 0 :
        char *index_space0 = strchr(name,' ');
        char str[sizeof(name)];
        strncpy(str,name,index_space0-name+1);
        printf("Thread 0 => My FirstName is : %s \n",str);
        break;
    case 1 :
        char *index_space1 = strchr(name,' ');
        printf("Thread 1 => My LastName is : %s \n",index_space1+1);
        break;
    case 2 :
        char reverse_name[50];
        strcpy(reverse_name,name);
        int item;
        for (int i =0;i<sizeof(name)/2;i++){
            item = reverse_name[i];
            reverse_name[i] = reverse_name[strlen(name)-i-1];
            reverse_name[strlen(name)-i-1] = item;
        }
        printf("Thread 2 => My ReverseName is : %s \n",reverse_name);
        break;
    case 3 :
        printf("Thread 3 => Count of Character in My Name is : %ld \n",strlen(name)-1);
        break;
    default:
        break;
    }
    pthread_exit(NULL);
}
int main(int argc,char *argv[]){
    pthread_t threads[NUM_THREADS];
    int rc;
    for(long i=0;i<NUM_THREADS;i++){
        printf("In Main => creating thread %ld\n",i);
        rc = pthread_create(&threads[i],NULL,doSometing,(void*)i);
        if (rc){
            printf("ERR: form create in %ld",i);
            exit(-1);
        }
    }
    pthread_exit(NULL);
}