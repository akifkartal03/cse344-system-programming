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
#define MAX_WRITE 4096
#define MAX_READ 1024
#define read 1
#define write 2

void openControlMutex();
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

char **queries;
sem_t *controlMutex;
sem_t *testMutex;
args givenParams;
node_t *head;
int recordSize = 0;
int activeWorkers = 0;
int queueSize = 0;
int AR = 0;
int AW = 0;
int WR = 0;
int WW = 0;
pthread_mutex_t busyMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t taskMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t okToDelegate = PTHREAD_COND_INITIALIZER;
pthread_cond_t okToExecute = PTHREAD_COND_INITIALIZER;
pthread_cond_t okToRead = PTHREAD_COND_INITIALIZER;
pthread_cond_t okToWrite = PTHREAD_COND_INITIALIZER;
int currentFd;
int main(int argc, char *argv[])
{
    //first check double instantiation
    openControlMutex();
    checkArguments(argc,argv,&givenParams);
    becomeDaemon();
    connectSignalHandler();
    writePid();
    initData();
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
void openControlMutex(){
    if ((controlMutex = sem_open("test", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        if (errno == EEXIST){
            printf("Only one instantiation can be created!\n");
            //closeControlMutex();
            exit(EXIT_FAILURE);
        }
        errExit("sem_open error!");
    }
}
void closeControlMutex(){
    if (sem_close(controlMutex) == -1)
        errExit("sem_close error!");
    if (sem_unlink("test") == -1)
        errExit("sem_unlink error!");
    /*if (sem_close(testMutex) == -1)
        errExit("sem_close error!");
    if (sem_unlink("deneme") == -1)
        errExit("sem_unlink error!");*/
}
void becomeDaemon(){
    switch (fork())
    {
        case -1:
            errExit("fork error!");
        case 0:
            break;
        default:
            exit(EXIT_SUCCESS);
    }

    if (setsid() == -1)
        errExit("setsid error!");

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    switch (fork())
    {
        case -1:
            errExit( "fork error!");
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
        closeControlMutex();
        dprintf(givenParams.logFd, "[%s] Termination signal received, waiting for ongoing threads to complete.\n", getTime());
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
}
void loadDataset(){
    clock_t start, end;
    start = clock();
    readFile(givenParams.datasetFd,&recordSize);
    end = clock();
    dprintf(givenParams.logFd, "[%s] Dataset loaded in %.5f seconds with %d records.\n",
            getTime(), (double)(end - start) / CLOCKS_PER_SEC, recordSize);
    printList2(head);
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
        errExit("socket error!");

    int val = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) == -1)
        errExit("setsockopt error!");
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(givenParams.port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");


    if (bind(socketfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        errExit("bind error!");

    if (listen(socketfd, 30) == -1)
        errExit("listen error!");

    while (1)
    {
        socklen_t addr_size = sizeof(struct sockaddr_in);
        if ((newFd = accept(socketfd, (struct sockaddr *)&newAddr, &addr_size)) == -1)
            errExit("accept error");
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
        currentFd = newFd; //add new query to the queue
        pthread_mutex_unlock(&taskMutex);

        pthread_cond_signal(&okToExecute); //signal any avaliable thread
    }
    exit(EXIT_SUCCESS);
}
void *sqlEngine(void *index){
    int *i = (int *)index;

    while(1){
        dprintf(givenParams.logFd, "[%s] Thread #%d: waiting for connection\n", getTime(), *i);
        if (isFinished())
            break;
        pthread_mutex_lock(&taskMutex);
        while (queueSize == 0) { // if no query just wait
            pthread_cond_wait(&okToExecute,&taskMutex);
        }
        if (isFinished()){
            pthread_mutex_unlock(&taskMutex);
            break;
        }
        pthread_mutex_lock(&busyMutex);
        activeWorkers++;
        pthread_mutex_unlock(&busyMutex);
        dprintf(givenParams.logFd, "[%s] A connection has been delegated to thread id #%d\n", getTime(), *i);
        //get element from queue
        pthread_mutex_unlock(&taskMutex);
        safeRead(currentFd,queries[*i],MAX_READ);
        dprintf(givenParams.logFd, "[%s] Thread #%d: received query '%s'\n", getTime(), *i,queries[*i]);
        if(getQueryType(*i) == read){
            reader(*i,currentFd);
        }
        else if(getQueryType(*i) == write){
            writer(*i,currentFd);
        }
        //sleep for 0.5 seconds
        milSleep(500);
        pthread_mutex_lock(&busyMutex);
        activeWorkers--;
        pthread_mutex_unlock(&busyMutex);
        pthread_cond_signal(&okToDelegate);

    }
    pthread_exit(NULL);
}
//take this to database
int getQueryType(int index){
    char tempQuery[strlen(queries[index]) + 1];
    strcpy(tempQuery,queries[index]);
    char *firstToken = strtok(queries[index]," ");
    if(firstToken != NULL){
        if(strcmp(firstToken,"SELECT") == 0)
            return read;
        else if(strcmp(firstToken,"UPDATE") == 0){
            return write;
        }
        else
            return -1;
    }
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
    char *result = mySelect(queries[index]);
    dprintf(givenParams.logFd, "[%s] Thread #%d: query completed, %d records have been returned.\n",
            getTime(), index, getReturnSize(result));
    safeWrite(fd,result,MAX_WRITE);
}
void updateDB(int index,int fd){
    int affected = update(queries[index]);
    int len = 0;
    if (affected > 0)
        len = (int)((ceil(log10(affected))+1)*sizeof(char));
    else
        len = 2;
    dprintf(givenParams.logFd, "[%s] Thread #%d: query completed, %d records have been affected.\n",
            getTime(), index, affected);
    char result[len + 1];
    sprintf(result, "%d", affected);
    safeWrite(fd,result,sizeof (result));
}