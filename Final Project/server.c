#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "helper.h"
#include "linked_list.h"
#include "sql_engine.h"
#include "queue.h"

#define SEM_NAME "double"
#define MAX 4096
#define MAX_READ 1024
#define read 1
#define write 2
#define TEMPFILE "171044098.pid"

void openControlFile();
void closeControlMutex();
void becomeDaemon();
void connectSignalHandler();
void exitHandler(int signal);
void writePid();
void loadDataset();
void printList2();
void serverMain();
void *sqlEngine(void *index); //thread function
int getQueryType(int index);
void reader(int index,int fd);
void writer(int index, int fd);
int isFinished();
void accessDB(int index, int fd);
void updateDB(int index, int fd);
void initData();
void createPool();

volatile __sig_atomic_t exitSignal = 0;
pthread_t *tids;
char **queries;
sem_t *controlMutex = NULL;
//sem_t *testMutex;
args givenParams;
int recordSize = 0;
int activeWorkers = 0;
int AR = 0;
int AW = 0;
int WR = 0;
int WW = 0;
pthread_mutex_t busyMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t taskMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t runMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t okToDelegate = PTHREAD_COND_INITIALIZER;
pthread_cond_t okToExecute = PTHREAD_COND_INITIALIZER;
pthread_cond_t okToRead = PTHREAD_COND_INITIALIZER;
pthread_cond_t okToWrite = PTHREAD_COND_INITIALIZER;
queue *queryQueue = NULL;

int main(int argc, char *argv[])
{
    //first check double instantiation
    openControlFile();
    checkArguments(argc,argv,&givenParams);
    becomeDaemon();
    connectSignalHandler();
    //writePid();
    initData();
    loadDataset();
    createPool();
    //loadDataset();
    //freeList(head);
    serverMain();
    /*testMutex = sem_open("deneme", O_CREAT | O_EXCL, 0666,0);
    while(1){
        sem_wait(testMutex);
        break;
    }*/

    return 0;
}
void openControlFile(){


    if(open(TEMPFILE, O_RDWR|O_CREAT|O_EXCL, 0666) == -1) {
        if (errno == EEXIST){
            printf("Only one instantiation can be created!\n");
            //closeControlMutex();
            exit(EXIT_FAILURE);
        }
    }

    /*if ((controlMutex = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        if (errno == EEXIST){
            printf("Only one instantiation can be created!\n");
            //closeControlMutex();
            exit(EXIT_FAILURE);
        }
        errExit("sem_open error!",1);
    }*/
}
void closeControlMutex(){
    remove(TEMPFILE);
    /*if (sem_close(controlMutex) == -1)
        errExit("sem_close error!",1);
    if (sem_unlink(SEM_NAME) == -1)
        errExit("sem_unlink error!",1);
    if (sem_close(testMutex) == -1)
        errExit("sem_close error!");
    if (sem_unlink("deneme") == -1)
        errExit("sem_unlink error!");*/
}
void becomeDaemon(){
    switch (fork())
    {
        case -1:
            errExit("fork error!",1);
        case 0:
            break;
        default:
            exit(EXIT_SUCCESS);
    }

    if (setsid() == -1)
        errExit("setsid error!",1);

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    switch (fork())
    {
        case -1:
            errExit( "fork error!",1);
        case 0:
            break;
        default:
            exit(EXIT_SUCCESS);
    }
    umask(0);

    for (int i = sysconf(_SC_OPEN_MAX); i >= 0; i--){
        if (i != givenParams.logFd && i != givenParams.datasetFd)
            close(i);
    }

}
void connectSignalHandler(){
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &exitHandler;
    sigaction(SIGINT, &sa, NULL);
}
void exitHandler(int signal){
    if (signal == SIGINT){
        exitSignal = 1;
        dprintf(givenParams.logFd, "[%s] Termination signal received, waiting for ongoing threads to complete.\n", getTime());
        addRear(queryQueue,1);
        pthread_cond_broadcast(&okToDelegate);
        pthread_cond_broadcast(&okToExecute);
        for (int i = 0; i < givenParams.poolSize; i++)
        {
            if (!pthread_equal(pthread_self(), tids[i]))
                pthread_join(tids[i], NULL);
        }
        free(tids);
        for (int i = 0; i < givenParams.poolSize; ++i) {
            free(queries[i]);
        }
        free(queries);
        freeList(head);
        freeQueue(queryQueue);
        closeControlMutex();
        dprintf(givenParams.logFd, "[%s] All threads have terminated, server shutting down.\n", getTime());
        close(givenParams.logFd);
        exit(EXIT_SUCCESS);
    }
}
void writePid(){
    dprintf(givenParams.logFd, "[%s] process pid :[%ld]\n", getTime(),(long)getpid());
}
void initData(){
    queries = (char**)calloc (givenParams.poolSize,sizeof(char*));
    for (int i = 0; i < givenParams.poolSize; ++i) {
        queries[i] = (char*) calloc(MAX_READ,sizeof(char));
    }
    queryQueue = createQueue();
    tids = (pthread_t*) malloc(givenParams.poolSize*sizeof (pthread_t));

}
void loadDataset(){
    dprintf(givenParams.logFd, "[%s] Loading dataset...\n",getTime());
    clock_t start, end;
    start = clock();
    readFile(givenParams.datasetFd,&recordSize);
    end = clock();
    dprintf(givenParams.logFd, "[%s] Dataset loaded in %.5f seconds with %d records.\n",
            getTime(), (double)(end - start) / CLOCKS_PER_SEC, recordSize);
    //printList2(head);
}

void printList2(){
    node_t *iter = head;
    while (iter != NULL)
    {
        dprintf(givenParams.logFd,"ColumnName:%s\n",iter->columnName);
        for (int i = 0; i < iter->size; ++i) {
            dprintf(givenParams.logFd,"%s, ",iter->data[i]);
        }
        dprintf(givenParams.logFd,"\n");
        iter = iter->next;
    }
}
void serverMain(){
    int socketfd;
    struct sockaddr_in serverAddr;

    int newFd;
    struct sockaddr_in newAddr;
    //create ipv4 socket
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        errExit("socket error!",1);

    int val = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) == -1)
        errExit("setsockopt error!",1);
    memset(&serverAddr, '\0', sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(givenParams.port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");


    if (bind(socketfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        errExit("bind error!",1);

    if (listen(socketfd, 30) == -1)
        errExit("listen error!",1);

    while (1)
    {
        socklen_t addr_size = sizeof(struct sockaddr_in);
        if ((newFd = accept(socketfd, (struct sockaddr *)&newAddr, &addr_size)) == -1)
            errExit("accept error",1);
        /*
        dprintf(givenParams.logFd,"connection accepted!\n");
        char buf[MAX];
        safeRead(newFd,buf,MAX);
        dprintf(givenParams.logFd,"[%s]read in server:%s\n",getTime(),buf);
        char test2[30] = "msg1\nmsg2\nmsg3";
        safeWrite(newFd,test2,sizeof(test2));*/
        //senkranizayson
        pthread_mutex_lock(&busyMutex);
        while (activeWorkers == givenParams.poolSize) { // if everyone is busy, wait
            dprintf(givenParams.logFd,"[%s]No thread is available! Waiting…\n",getTime());
            pthread_cond_wait(&okToDelegate,&busyMutex);
        }
        pthread_mutex_unlock(&busyMutex);
        pthread_mutex_lock(&taskMutex);
        addRear(queryQueue,newFd);
        //currentFd = newFd; //add new query to the queue
        pthread_mutex_unlock(&taskMutex);

        pthread_cond_signal(&okToExecute); //signal any avaliable thread
    }
    exit(EXIT_SUCCESS);
}
void *sqlEngine(void *index){
    int *i = (int *)index;
    pthread_mutex_lock(&runMutex);
    pthread_mutex_unlock(&runMutex);
    while(1){
        dprintf(givenParams.logFd, "[%s] Thread #%d: waiting for connection\n", getTime(), *i);
        if (exitSignal)
            break;
        pthread_mutex_lock(&taskMutex);
        while (isQueueEmpty(queryQueue)) { // if no query just wait
            pthread_cond_wait(&okToExecute,&taskMutex);
        }
        if (exitSignal){
            pthread_mutex_unlock(&taskMutex);
            break;
        }
        pthread_mutex_lock(&busyMutex);
        activeWorkers++;
        pthread_mutex_unlock(&busyMutex);
        dprintf(givenParams.logFd, "[%s] A connection has been delegated to thread id #%d\n", getTime(), *i);
        int currentFd = removeFront(queryQueue);
        pthread_mutex_unlock(&taskMutex);
        while(safeRead(currentFd,queries[(*i)-1],MAX_READ,1)!=0){
            int cap = 1024;
            while (queries[(*i)-1][strlen(queries[(*i)-1])-1] != '\n'
                && queries[(*i)-1][strlen(queries[(*i)-1])] != '\0'){
                cap = cap + 1024;
                queries[(*i)-1] = realloc(queries[(*i)-1], cap * sizeof(char));
                safeRead(currentFd,queries[(*i)-1],MAX_READ,1);
            }
            dprintf(givenParams.logFd, "[%s] Thread #%d: received query '%s'\n",
                    getTime(), *i,queries[(*i)-1]);
            if(getQueryType(*i) == read){
                reader(*i,currentFd);
            }
            else if(getQueryType(*i) == write){
                writer(*i,currentFd);
            }
            //sleep for 0.5 seconds
            milSleep(500);
        }

        pthread_mutex_lock(&busyMutex);
        activeWorkers--;
        pthread_mutex_unlock(&busyMutex);
        pthread_cond_signal(&okToDelegate);

    }

    free(i);
    return NULL;
}
//take this to database
int getQueryType(int index){
    return getQueryTypeEngine(queries[index-1]);
}
void reader(int index,int fd){
    pthread_mutex_lock(&m);
    while ((AW + WW) > 0) { // if writers, wait
        WR++;
        pthread_cond_wait(&okToRead,&m);
        WR--;
    }
    AR++;
    pthread_mutex_unlock(&m);
    accessDB(index,fd);
    pthread_mutex_lock(&m);
    AR--;
    if (AR == 0 && WW > 0)
        pthread_cond_signal(&okToWrite);
    pthread_mutex_unlock(&m);

}
void writer(int index,int fd){
    pthread_mutex_lock(&m);
    while ((AW + AR) > 0) {
        WW++;
        pthread_cond_wait(&okToWrite,&m);
        WW--;
    }
    AW++;
    pthread_mutex_unlock(&m);
    updateDB(index,fd);
    pthread_mutex_lock(&m);
    AW--;
    if (WW > 0) // give priority to other writers
        pthread_cond_signal(&okToWrite);
    else if (WR > 0)
        pthread_cond_broadcast(&okToRead);
    pthread_mutex_unlock(&m);
}

void accessDB(int index,int fd){
    char *result = NULL;
    result = mySelect(queries[index-1]);
    dprintf(givenParams.logFd, "[%s] Thread #%d: query completed, %d records have been returned.\n",
            getTime(), index, getReturnSize(result));
    safeWrite(fd,result, strlen(result),1);

    /*if(strlen(result) >= MAX_WRITE){
        int i = 0;
        while(strlen(&result[i*MAX_WRITE]) >= MAX_WRITE){
            safeWrite(fd,&result[i*MAX_WRITE],MAX_WRITE,1);
            i++;
        }
    }
    else{
        safeWrite(fd,result,MAX_WRITE,1);
    }*/

    free(result);
}
void updateDB(int index,int fd){
    int affected = update(queries[index-1]);
    int len = 0;
    if (affected > 0)
        len = (int)((ceil(log10(affected))+1)*sizeof(char));
    else
        len = 2;
    dprintf(givenParams.logFd, "[%s] Thread #%d: query completed, %d records have been affected.\n",
            getTime(), index, affected);
    char result[len + 1];
    sprintf(result, "%d\n", affected);
    safeWrite(fd,result,strlen (result),1);
}
void createPool(){
    int n = givenParams.poolSize;
    pthread_mutex_lock(&runMutex);
    for (int i = 1; i <= n; ++i) {
        int *index = (int*) calloc(1,sizeof(int));
        *index = i;
        pthread_create(&tids[i-1], NULL, sqlEngine, index);
    }
    dprintf(givenParams.logFd, "[%s] A pool of %d threads has been created\n",
            getTime(), n);
    pthread_mutex_unlock(&runMutex);
}