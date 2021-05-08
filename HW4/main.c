#include "helper.h"
#include "queue.h"

int isFinished;
double money;
sem_t run,busy,dataMutex;
sem_t mutex,empty,full;
int N, fdHw, fdStd;


void *threadG(void *data);
void *threadStd(void *info);
int findStudent(const student hiredStds[], char priority);
int main(int argc, char *argv[])
{
    /*Check arguments*/
    args givenParams;
    checkArguments(argc, argv, &givenParams);

    /*init important variables*/
    fdHw = safeOpen(givenParams.hwFile, O_RDONLY);
    fdStd = safeOpen(givenParams.studentFile, O_RDONLY);
    money = givenParams.money;
    N = getNumberOfLine(fdStd);
    isFinished = 0;

    pthread_t tids[N];
    pthread_t idG;
    student hiredStds[N];
    sem_init(&run, 0, 0);
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, 10);
    sem_init(&full, 0, 0);
    sem_init(&busy, 0, 0);
    sem_init(&dataMutex, 0, 1);

    //printf("N:%d\n",N);
    //printf("money:%.2f\n",money);
    /*set attributes for G thread*/
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    /*create G thread*/
    queue *stdQueue = createQueue();
    pthread_create(&idG, &attr, threadG, (void *)stdQueue);
    /*wait for queue to fill and initialize*/
    /*if (sem_wait(&run) == -1)
        errExit("sem_wait");*/

    pthread_attr_destroy(&attr);

    /*create hired students threads and init them*/
    for (int i = 0; i < N; i++)
    {
        pthread_create(&tids[i], NULL, threadStd, &hiredStds[i]);
    }
    initStudents(hiredStds, fdStd, N, tids);
    mainPrintStudents(hiredStds,N);
    if (sem_post(&run) == -1)
        errExit("sem_post");

    /*Choose student an make hwk*/
    int index;
    while (money > 0 && !isFinished)
    {
        if (sem_wait(&full) == -1)
            errExit("sem_wait");
        if (sem_wait(&mutex) == -1)
            errExit("sem_wait");
        if (!isQueueEmpty(stdQueue)){
            index = findStudent(hiredStds, getFront(stdQueue));
            if (index == -1)
            {
                //all of them are busy
                int counter;
                if (sem_getvalue(&busy, &counter) == -1)
                    errExit("sem_get");
                if (counter > 0){
                    if (sem_wait(&busy) == -1)
                        errExit("sem_wait");
                }
                if (sem_wait(&busy) == -1)
                    errExit("sem_wait");
                index = findStudent(hiredStds, getFront(stdQueue));
            }
            if (hiredStds[index].price <= money)
            {
                if (sem_wait(&dataMutex) == -1)
                    errExit("sem_wait");
                hiredStds[index].currentHw = removeFront(stdQueue);
                hiredStds[index].isNotified += 1;
                money = money - hiredStds[index].price;
                //hiredStds[index].isBusy = 1;
                hiredStds[index].income += hiredStds[index].price;
                hiredStds[index].solvedCount += 1;
                if (sem_post(hiredStds[index].notify) == -1)
                    errExit("sem_post");

                if (sem_post(&dataMutex) == -1)
                    errExit("sem_post");

                if (sem_wait(hiredStds[index].startSolve) == -1)
                    errExit("sem_wait");


            }
            else
            {
                isFinished = 1;
                mainNoMoneyMsg();
                if (sem_post(&mutex) == -1)
                    errExit("sem_post");
                if (sem_post(&empty) == -1)
                    errExit("sem_post");
                break;
            }
            if (sem_post(&mutex) == -1)
                errExit("sem_post");
            if (sem_post(&empty) == -1)
                errExit("sem_post");
        }
        else{
            break;
        }
    }
    if (isQueueEmpty(stdQueue)){
        mainNoHwMsg();
    }
    isFinished = 1;
    for (int i = 0; i < N; ++i) {
        if (sem_post(hiredStds[i].notify) == -1)
            errExit("sem_post");
    }
    /*join for all students*/
    for (int i = 0; i < N; i++)
    {
        if (!pthread_equal(pthread_self(), tids[i]))
            pthread_join(tids[i], NULL);
    }
    mainReportMsg(hiredStds,N,money);

    /*free resources and exit*/
    freeQueue(stdQueue);
    sem_destroy(&run);
    sem_destroy(&mutex);
    destroy(hiredStds,N);
    printf("Main finishes...\n");
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
            if (sem_wait(&empty) == -1)
                errExit("sem_wait");
            if (sem_wait(&mutex) == -1)
                errExit("sem_wait");
            if (isFinished)
                break;

            gNewHwMsg(nextHw,money);
            addRear(p, nextHw);

            if (sem_post(&mutex) == -1)
                errExit("sem_post");
            if (sem_post(&full) == -1)
                errExit("sem_post");
        }

    } while (nextHw != 'x' && !isFinished);
    if (isFinished)
        gNoMoneyMsg();
    else
        gNoHwMsg();
    if (sem_post(&mutex) == -1)
        errExit("sem_post");
    if (sem_post(&full) == -1)
        errExit("sem_post");
    return NULL;
}
/*Student for hire thread function*/
void *threadStd(void *info)
{
    if (sem_wait(&run) == -1)
        errExit("sem_wait");
    if (sem_post(&run) == -1)
        errExit("sem_post");
    student *this = (student *)info;
    while (!isFinished || this->isNotified)
    {
        stdWaitMsg(this->name);
        if (sem_wait(this->notify) == -1)
            errExit("sem_wait");
        if (this->isNotified){
            if (sem_wait(&dataMutex) == -1)
                errExit("sem_wait");
            this->isBusy = 1;
            stdSolvingMsg(this->name,this->price,money,this->currentHw);
            if (sem_post(this->startSolve) == -1)
                errExit("sem_post");
            if (sem_post(&dataMutex) == -1)
                errExit("sem_post");

            sleep(6 - (int)this->speed);

            if (sem_wait(&dataMutex) == -1)
                errExit("sem_wait");
            this->isNotified -= 1;
            this->isBusy = 0;

            if (sem_post(&dataMutex) == -1)
                errExit("sem_post");
            int counter;
            if (sem_getvalue(&busy, &counter) == -1)
                errExit("sem_get");
            if (counter == 0){
                if (sem_post(&busy) == -1)
                    errExit("sem_post");
            }
        }
        else
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
