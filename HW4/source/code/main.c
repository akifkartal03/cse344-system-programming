#include "helper.h"
#include "queue.h"


volatile __sig_atomic_t exitSignal = 0;

int isFinished;
double money;
sem_t run,busy,dataMutex;
sem_t mutex,empty,full;
int N, fdHw, fdStd;


void *threadG(void *data);
void *threadStd(void *info);
int findStudent(const student hiredStds[], char priority);
void exitHandler(int signal);
void cleanAndExit(student stds[],pthread_t ids[],int n,queue *head);
int safeSemWait(sem_t *sem);
int main(int argc, char *argv[])
{
    /*CTRL-C signal handling with sigaction*/
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &exitHandler;
    sigaction(SIGINT, &sa, NULL);
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
    if (exitSignal)
        cleanAndExit(hiredStds,tids,N,stdQueue);
    if (sem_post(&run) == -1)
        errExit("sem_post");

    /*Choose student an make hwk*/
    int index;
    while (money > 0 && !isFinished)
    {
        if (exitSignal)
            cleanAndExit(hiredStds,tids,N,stdQueue);
        if (safeSemWait(&full) == -1)
            cleanAndExit(hiredStds,tids,N,stdQueue);
        if (exitSignal)
            cleanAndExit(hiredStds,tids,N,stdQueue);
        if (safeSemWait(&mutex) == -1)
            cleanAndExit(hiredStds,tids,N,stdQueue);
        if (exitSignal)
            cleanAndExit(hiredStds,tids,N,stdQueue);
        if (!isQueueEmpty(stdQueue)){
            if (exitSignal)
                cleanAndExit(hiredStds,tids,N,stdQueue);
            index = findStudent(hiredStds, getFront(stdQueue));
            if (index == -1)
            {
                //all of them are busy
                int counter;
                if (sem_getvalue(&busy, &counter) == -1)
                    errExit("sem_get");
                if (counter > 0){
                    if (safeSemWait(&busy) == -1)
                        cleanAndExit(hiredStds,tids,N,stdQueue);
                }
                if (exitSignal)
                    cleanAndExit(hiredStds,tids,N,stdQueue);
                if (safeSemWait(&busy) == -1)
                    cleanAndExit(hiredStds,tids,N,stdQueue);
                index = findStudent(hiredStds, getFront(stdQueue));
            }
            if (exitSignal)
                cleanAndExit(hiredStds,tids,N,stdQueue);
            if (hiredStds[index].price <= money)
            {
                if (exitSignal)
                    cleanAndExit(hiredStds,tids,N,stdQueue);
                if (safeSemWait(&dataMutex) == -1)
                    cleanAndExit(hiredStds,tids,N,stdQueue);
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

                if (exitSignal)
                    cleanAndExit(hiredStds,tids,N,stdQueue);
                if (safeSemWait(hiredStds[index].startSolve) == -1)
                    cleanAndExit(hiredStds,tids,N,stdQueue);

                if (exitSignal)
                    cleanAndExit(hiredStds,tids,N,stdQueue);
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
        if (exitSignal)
            cleanAndExit(hiredStds,tids,N,stdQueue);
    }
    if (isQueueEmpty(stdQueue)){
        mainNoHwMsg();
    }
    if (exitSignal)
        cleanAndExit(hiredStds,tids,N,stdQueue);
    isFinished = 1;
    for (int i = 0; i < N; ++i) {
        if (sem_post(hiredStds[i].notify) == -1)
            errExit("sem_post");
    }
    if (exitSignal)
        cleanAndExit(hiredStds,tids,N,stdQueue);
    /*join for all students*/
    for (int i = 0; i < N; i++)
    {
        if (exitSignal)
            cleanAndExit(hiredStds,tids,N,stdQueue);
        if (!pthread_equal(pthread_self(), tids[i]))
            pthread_join(tids[i], NULL);
    }
    mainReportMsg(hiredStds,N,money);

    /*free resources and exit*/
    freeQueue(stdQueue);
    sem_destroy(&run);
    sem_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&dataMutex);
    sem_destroy(&busy);
    sem_destroy(&empty);
    destroy(hiredStds,N);
    return 0;
}
/*G thread function*/
void *threadG(void *data)
{
    queue *p = (queue *)data;
    char nextHw;
    do
    {
        if (exitSignal)
            pthread_exit(NULL);
        nextHw = readOneChar(fdHw);
        if (nextHw != 'x' && !isFinished)
        {
            if (exitSignal)
                pthread_exit(NULL);
            if (safeSemWait(&empty) == -1)
                pthread_exit(NULL);
            if (exitSignal)
                pthread_exit(NULL);
            if (safeSemWait(&mutex) == -1)
                pthread_exit(NULL);
            if (isFinished)
                break;
            if (exitSignal)
                pthread_exit(NULL);

            gNewHwMsg(nextHw,money);
            addRear(p, nextHw);

            if (sem_post(&mutex) == -1)
                errExit("sem_post");
            if (sem_post(&full) == -1)
                errExit("sem_post");
        }
        if (exitSignal)
            pthread_exit(NULL);

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
    if (exitSignal)
        pthread_exit(NULL);
    if (safeSemWait(&run) == -1)
        pthread_exit(NULL);
    if (sem_post(&run) == -1)
        errExit("sem_post");
    if (exitSignal)
        pthread_exit(NULL);
    student *this = (student *)info;
    while (!isFinished || this->isNotified)
    {
        if (exitSignal)
            pthread_exit(NULL);
        stdWaitMsg(this->name);
        if (safeSemWait(this->notify) == -1)
            pthread_exit(NULL);
        if (exitSignal)
            pthread_exit(NULL);
        if (this->isNotified){
            if (exitSignal)
                pthread_exit(NULL);
            if (safeSemWait(&dataMutex) == -1)
                pthread_exit(NULL);
            this->isBusy = 1;
            stdSolvingMsg(this->name,this->price,money,this->currentHw);
            if (sem_post(this->startSolve) == -1)
                errExit("sem_post");
            if (sem_post(&dataMutex) == -1)
                errExit("sem_post");
            if (exitSignal)
                pthread_exit(NULL);

            sleep(6 - (int)this->speed);

            if (safeSemWait(&dataMutex) == -1)
                pthread_exit(NULL);
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
        if (exitSignal)
            pthread_exit(NULL);
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
        if (exitSignal)
            return -1;
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
void exitHandler(int signal)
{
    if (signal == SIGINT)
    {
        exitSignal = 1;
    }
}
void cleanAndExit(student stds[],pthread_t ids[],int n,queue *head){
    printf("Termination signal received, closing.\n");
    if (sem_post(&dataMutex) == -1)
        errExit("sem_post");
    if (sem_post(&mutex) == -1)
        errExit("sem_post");
    if (sem_post(&empty) == -1)
        errExit("sem_post");
    for (int i = 0; i < n; ++i) {
        if (sem_post(stds[i].notify) == -1)
            errExit("sem_post");
    }
    for (int i = 0; i < n; i++)
    {
        if (!pthread_equal(pthread_self(), ids[i]))
            pthread_join(ids[i], NULL);
    }
    freeQueue(head);
    sem_destroy(&run);
    sem_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&dataMutex);
    sem_destroy(&busy);
    sem_destroy(&empty);
    destroy(stds,n);
    exit(EXIT_SUCCESS);
}
int safeSemWait(sem_t *sem){
    if (sem_wait(sem) == -1){
        if (errno == EINTR && exitSignal == 1){
            return -1;
        } else{
            errExit("sem_wait interrupt");
        }
    }
    return 0;
}