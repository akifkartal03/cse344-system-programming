#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <pthread.h>
#include "linked_list.h"

volatile __sig_atomic_t exitSignal = 0;
node_t *head = NULL;
sem_t run;

typedef struct a{
    int a;
    int b;
}mydata;

void exitHandler(int signal)
{
    if (signal == SIGINT)
    {
        exitSignal = 1;
    }
}

void* foo(void *data){
    sem_wait(&run);
    printf("Thread fooo\n");
    sem_post(&run);
    return NULL;
}

void* addElement(void *data){
    printf("Thread1\n");
    mydata* p = (mydata*) data;
    for (int i = 2; i < 10; i++)
    {
        head = addLast(head,i);
    }
    p->a =18;
    p->b =23;
    sem_post(&run);
    return NULL;
}
void* removeElement(void *data){
    sem_wait(&run);
    printf("Thread2\n");
    removeNode(head,8);
    removeNode(head,5);

    sem_post(&run);
    return NULL;
}
void* findElement(void *unused){
    sem_wait(&run);
    printf("Thread3\n");
    printf("%d\n",findNode(head,3)->data);

    sem_post(&run);
    return NULL;
}
void* printMyList(void *unused){
    sem_wait(&run);
    printf("Thread4\n");
    printList(head);

    sem_post(&run);
    return NULL;
}
int main(int argc, char const *argv[])
{
    /*CTRL-C signal handling with sigaction*/
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &exitHandler;
    sigaction(SIGINT, &sa, NULL);

    pthread_t id1,id2,id3,id4,id5;
    sem_init(&run,0,0);
    mydata data1;
    pthread_create(&id1,NULL,&addElement,&data1);
    sem_wait(&run);
    if (exitSignal)
    {
        printf("You are exiting...\n");
        freeList(head);
        exit(EXIT_SUCCESS);
    }
    pthread_create(&id5,NULL,&foo,NULL);
    pthread_create(&id2,NULL,&removeElement,&data1);
    pthread_create(&id3,NULL,&findElement,NULL);
    pthread_create(&id4,NULL,&printMyList,NULL);
    sem_post(&run);
    while (1){
        if (exitSignal)
        {
            printf("You are exiting...\n");
            //sem_post(&run);
            //pthread_exit(NULL);
            if (!pthread_equal (pthread_self (), id1))
                pthread_join(id1,NULL);
            if (!pthread_equal (pthread_self (), id2))
                pthread_join(id2,NULL);
            if (!pthread_equal (pthread_self (), id3))
                pthread_join(id3,NULL);
            if (!pthread_equal (pthread_self (), id4))
                pthread_join(id4,NULL);
            if (!pthread_equal (pthread_self (), id5))
                pthread_join(id5,NULL);
            freeList(head);
            sem_destroy(&run);
            exit(EXIT_SUCCESS);
        }
    }

    freeList(head);
    sem_destroy(&run);
    printf("Main finishes...\n");
    return 0;
}
