#include "helper.h"
#include "queue.h"


int isFinished;
int allBusy;
queue *stdQueue;
double money;
sem_t run;
sem_t mutex;
int N ,fdHw ,fdStd;

void *threadG(void *unused);
void *threadStd(void *info);
int findStudent(const student hiredStds[],char priority);
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
    stdQueue = createQueue();
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

    /*Choose student an make hwk*/
    int index;
    while (!isQueueEmpty(stdQueue) && money > 0)
    {
       index = findStudent(hiredStds,getFront(stdQueue));
       if (index == -1)
       {
           allBusy = 1;
           while (allBusy);
           index = findStudent(hiredStds,getFront(stdQueue));
       }
       removeFront(stdQueue);
       hiredStds[index].isNotified = 1;
       money = money - hiredStds[index].price;
    }

    /*join for all students*/
    for (int i = 0; i < N; i++)
    {
        if (!pthread_equal(pthread_self (), tids[i]))
            pthread_join(tids[i],NULL);
    }

    /*free resources and exit*/
    freeQueue(stdQueue);
    sem_destroy(&run);
    sem_destroy(&mutex);
    return 0;
}
void *threadG(void *unused){
    char nextHw;
    do
    {
       nextHw = readOneChar(fdHw);
       if (nextHw != 'x')
       {
           addRear(stdQueue,nextHw);
       }
       
    } while (nextHw != 'x');
    sem_post(&run);
    return NULL;
    
}

int findStudent(const student hiredStds[],char priority){
    double min = DBL_MAX;
    double max = -1;
    int index = -1;
    for (int i = 0; i < N; i++)
    {
        switch (priority)
        {
        case 'C':
            if ((hiredStds[i].price < min || index == -1) && !hiredStds[i].isBusy)
            {
                min = hiredStds[i].price;
                index = i;
            }
            break;
        case 'Q':
            if ((hiredStds[i].quality > max || index == -1) && !hiredStds[i].isBusy)
            {
                max = hiredStds[i].quality;
                index = i;
            }
            break;
        case 'S':
            if ((hiredStds[i].speed > max || index == -1) && !hiredStds[i].isBusy)
            {
                max = hiredStds[i].speed;
                index = i;
            }
            break;
        default:
            break;
        }
    
    }
    return index;
}
