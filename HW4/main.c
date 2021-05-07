#include "helper.h"
#include "queue.h"


int isFinished;
queue *stdQueue;
double money;
//Notify Array
sem_t run;
sem_t mutex;
int N ,fdHw ,fdStd;

void *threadG(void *unused);
void *threadStd(void *info);

int main(int argc, char const *argv[])
{
    /*Check argumnets*/
    args givenParams;
    checkArguments(argc, argv, &givenParams);

    /*init important variables*/
    fdHw = safeOpen(givenParams.hwFile, O_RDONLY);
    fdStd = safeOpen(givenParams.studentFile, O_RDONLY);
    money = givenParams.money;
    N = getNumberOfLine(fdStd);
    pthread_t tids[N];
    pthread_t idG;
    student hiredStds[N];
    sem_init(&run,0,0);
    sem_init(&mutex,0,1);

    /*set attributes for G thread*/
    pthread_attr_t attr;
    pthread_attr_init(&attr); 
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    /*create g thread*/
    pthread_create(&idG,&attr,&threadG,NULL);
    pthread_attr_destroy(&attr);

    /*wait for queue to fill and initalize*/
    if (sem_wait(run) == -1)
        errExit("sem_wait");
    
    /*create hired students threads and init them*/
    for (int i = 0; i < N; i++)
    {
        pthread_create(&tids[i],NULL,&threadStd,&hiredStds[i]);
    }
    initStudents(hiredStds,fdStd,tids);

    while (!isQueueEmpty(stdQueue) && money > 0)
    {
        
    }
    

    
    return 0;
}
