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
#include "queue.h"


//node_t *head = NULL;
sem_t run;

typedef struct a{
    int a;
    int b;
}mydata;

void* foo(void *data){
    printf("Thread1\n");
    queue* p = (queue*) data;
    addRear(p,'C');
    addRear(p,'Q');
    addRear(p,'S');
    addRear(p,'S');
    addRear(p,'C');
    addRear(p,'Q');
    addRear(p,'Q');
    removeFront(p);
    removeFront(p);
    printf("size:%d\n",p->size);
    sem_post(&run);
    return NULL;
}
/*
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
    mydata* p1 = (mydata*) data;
    removeNode(head,8);
    removeNode(head,5);
    printf("thr2 a:%d\n",p1->a);
    printf("thr2 b:%d\n",p1->b);
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
}*/
int main(int argc, char const *argv[])
{
    pthread_t id1,id2,id3,id4;
    sem_init(&run,0,0);
    mydata data1;
    queue *head = createQueue();
    pthread_create(&id1,NULL,&foo,(void*)head);
    sem_wait(&run);
    printQueue(head);
    if (!pthread_equal (pthread_self (), id1))
        pthread_join(id1,NULL);
    /*pthread_create(&id2,NULL,&removeElement,&data1);
    data1.a = 56;
    data1.b = 61;
    sem_post(&run);
    pthread_create(&id3,NULL,&findElement,NULL);
    pthread_create(&id4,NULL,&printMyList,NULL);
    if (!pthread_equal (pthread_self (), id1))
        pthread_join(id1,NULL);
    if (!pthread_equal (pthread_self (), id2))
        pthread_join(id2,NULL);
    if (!pthread_equal (pthread_self (), id3))
        pthread_join(id3,NULL);
    if (!pthread_equal (pthread_self (), id4))
        pthread_join(id4,NULL);
    freeList(head);
    sem_destroy(&run);*/
    freeQueue(head);
    printf("Main finishes...\n");
    return 0;
}
