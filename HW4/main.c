#include "helper.h"
#include "queue.h"

int isFinished;
int allBusy;
double money;
sem_t run;
sem_t mutex;
int N, fdHw, fdStd;

void *threadG(void *data);
void *threadStd(void *info);
int findStudent(const student hiredStds[], char priority);
int main(int argc, char *argv[])
{
    /*Check argumnets*/
    args givenParams;
    checkArguments(argc, argv, &givenParams);

    /*init important variables*/
    fdHw = safeOpen(givenParams.hwFile, O_RDONLY);
    fdStd = safeOpen(givenParams.studentFile, O_RDONLY);
    money = givenParams.money;
    N = getNumberOfLine(fdStd);
    isFinished = 0;
    allBusy = 0;
    pthread_t tids[N];
    pthread_t idG;
    student hiredStds[N];
    sem_init(&run, 0, 0);
    sem_init(&mutex, 0, 1);

    /*set attributes for G thread*/
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    /*create G thread*/
    queue *stdQueue = createQueue();
    pthread_create(&idG, &attr, &threadG, (void *)stdQueue);
    /*wait for queue to fill and initalize*/
    if (sem_wait(&run) == -1)
        errExit("sem_wait");

    pthread_attr_destroy(&attr);

    /*create hired students threads and init them*/
    for (int i = 0; i < N; i++)
    {
        pthread_create(&tids[i], NULL, &threadStd, &hiredStds[i]);
    }
    initStudents(hiredStds, fdStd, tids);

    /*Choose student an make hwk*/
    int index;
    while (!isQueueEmpty(stdQueue) && money > 0)
    {
        index = findStudent(hiredStds, getFront(stdQueue));
        if (index == -1)
        {
            allBusy = 1;
            while (allBusy)
                ;
            index = findStudent(hiredStds, getFront(stdQueue));
        }
        if (hiredStds[index].price <= money)
        {
            removeFront(stdQueue);
            hiredStds[index].isNotified = 1;
            money = money - hiredStds[index].price;
        }
        else
        {
            isFinished = 1;
            break;
        }
    }

    /*join for all students*/
    for (int i = 0; i < N; i++)
    {
        if (!pthread_equal(pthread_self(), tids[i]))
            pthread_join(tids[i], NULL);
    }

    /*free resources and exit*/
    freeQueue(stdQueue);
    sem_destroy(&run);
    sem_destroy(&mutex);
    return 0;
}
/*G thread function*/
void *threadG(void *data)
{
    queue *p = (queue *)data;
    char nextHw;
    do
    {
        nextHw = readOneChar(fdHw);
        if (nextHw != 'x' && !isFinished)
        {
            addRear(p, nextHw);
            sem_post(&run);
        }

    } while (nextHw != 'x' && !isFinished);
    return NULL;
}
/*Student for hire thread function*/
void *threadStd(void *info)
{
    student *this = (student *)info;
    while (1)
    {
        while (!this->isNotified && !isFinished)
            ;
        if (isFinished)
            break;
        this->isBusy = 1;
        sleep(6 - this->speed);
        this->isNotified = 0;
        this->isBusy = 0;
        if (isFinished)
            break;
    }
    return NULL;
}
int findStudent(const student hiredStds[], char priority)
{
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
