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
#include "linked_list.h"
#define MAX 1024
typedef struct da{
    char *query;
}req;
void openControlMutex();
void closeControlMutex();
void becomeDaemon();
void connectSignalHandler();
void exitHandler(int signal);
void writePid();
void loadDataset();
void printList2();
node_t *readFile(int fd);
void serverMain();

sem_t *controlMutex;
sem_t *testMutex;
args givenParams;
node_t *head;
int recordSize = 0;
int main(int argc, char *argv[])
{
    //first check double instantiation
    openControlMutex();
    checkArguments(argc,argv,&givenParams);
    becomeDaemon();
    connectSignalHandler();
    writePid();
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
void loadDataset(){
    clock_t start, end;
    start = clock();
    head = readFile(givenParams.datasetFd);
    end = clock();
    dprintf(givenParams.logFd, "[%s] Dataset loaded in %.5f seconds with %d records.\n",
            getTime(), (double)(end - start) / CLOCKS_PER_SEC, recordSize);
    printList2(head);
}
node_t *readFile(int fd){
    node_t *temp = NULL;
    int offset = 0;
    int bytes_read;
    int capacity = 50;
    int isFirst = 1;
    int i = 0;
    char c;
    char * parsed;
    char *buffer = (char *)calloc(50, sizeof(char));
    do
    {
        bytes_read = safeRead(fd, &c, 1);
        offset += bytes_read;
        if (capacity <= offset + 1)
        {
            capacity = capacity + 20;
            buffer = realloc(buffer, capacity * sizeof(char));
        }
        if(c != '\n' && bytes_read == 1){
            buffer[i] = c;
            i++;
        }

        else{
            /*if (!isFirst && bytes_read == 1)
                recordSize++;*/
            if(bytes_read == 1){
                buffer[i] = '\0';

                //printf ("bufer:%s\n",buffer);
                //printf("hreeeeeee\n");
                //char* tmp = strdup(buffer);
                //char temp[capacity];
                //strcpy(temp,buffer);
                //printf ("temp:%s\n",temp);
                int j = 0;
                parsed = strtok (buffer,",");
                //printf ("%s,",parsed);
                while (parsed != NULL)
                {
                    if (isFirst){
                        temp = addLast(temp,parsed,0,10);
                        //printf ("parsed:%s\n",parsed);
                    }
                    else{
                        //printf("iter:%s\n",iter->next->columnName);
                        //printf ("head:%s\n",head->columnName);
                        node_t *node = findByIndex(temp,j);

                        if (node->capacity <= node->size + 1){
                            node->capacity = node->capacity + 10;
                            node->data = realloc(node->data, node->capacity * sizeof(char*));
                        }
                        node->data[node->size] = (char*) calloc(strlen(parsed)+1,sizeof(char));
                        strcpy(node->data[node->size],parsed);
                        node->size = node->size + 1;
                        recordSize++;
                        //printf("size:%d\n",node->size);
                        //printf("size:%d\n",node->capacity);
                    }
                    //printf ("%s,",parsed);
                    j++;
                    parsed = strtok (NULL, ",");

                }

                //printf("\n");
                free(buffer);
                buffer = (char *)calloc(50, sizeof(char));
                capacity = 50;
                i = 0;
                offset = 0;
                if (isFirst){
                    isFirst = 0;
                    //printList(head);
                }
            }
        }

    } while (bytes_read == 1);
    free(buffer);
    return temp;

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
        dprintf(givenParams.logFd,"connection accepted!\n");
        char buf[MAX];
        safeRead(newFd,buf,MAX);
        dprintf(givenParams.logFd,"read in server:%s\n",buf);
        char test2[30] = "msg1\nmsg2\nmsg3";
        safeWrite(newFd,test2,sizeof(test2));
        //senkranizayson
    }
    exit(EXIT_SUCCESS);
}