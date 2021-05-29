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
#include "helper.h"
void openControlMutex();
void closeControlMutex();
sem_t *controlMutex;
args givenParams;
int main(int argc, char *argv[])
{
    //first check double instantiation
    openControlMutex();
    checkArguments(argc,argv,&givenParams);
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
}