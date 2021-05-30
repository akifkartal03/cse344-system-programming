#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <errno.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "helper.h"
void openControlMutex();
void closeControlMutex();
void becomeDaemon();
void connectSignalHandler();
void exitHandler(int signal);
void writePid();

sem_t *controlMutex;
sem_t *testMutex;
args givenParams;
int main(int argc, char *argv[])
{
    //first check double instantiation
    openControlMutex();
    checkArguments(argc,argv,&givenParams);
    becomeDaemon();
    connectSignalHandler();
    writePid();
    /*testMutex = sem_open("deneme", O_CREAT | O_EXCL, 0666,0);
    while(1){
        sem_wait(testMutex);
        break;
    }*/
    // TODO
    //impelement become deamon
    //implement signal handler
    //sonra test et
    //socket için advanced linux'a bakabilirsin...
    return 0;
}
void openControlMutex(){
    if ((controlMutex = sem_open("test", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
        if (errno == EEXIST){
            printf("Only one instantiation can be created!\n");
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
    if (sem_close(testMutex) == -1)
        errExit("sem_close error!");
    if (sem_unlink("deneme") == -1)
        errExit("sem_unlink error!");
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
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--){
        if (x != givenParams.logFd && x != givenParams.datasetFd)
            close(x);
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