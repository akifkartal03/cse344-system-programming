#ifndef _HELPER_H_
#define _HELPER_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <pthread.h>
#include <float.h>
#include <limits.h>


typedef struct arg
{

    char hwFile[41];
    char studentFile[41];
    double money;

} args;

typedef struct info
{
    char name[41];
    double price;
    double speed;
    double quality;
    double income;
    int solvedCount;
    int index;
    int isBusy;
    int isNotified;
    char currentHw;
    pthread_t id;
} student;

/*Generic functions coming from through all hwks*/
void checkArguments(int argc, char **argv, args *givenArgs);
void showUsageAndExit();
void errExit(char *msg);
int safeLseek(int fd, int offset, int whence);
int safeRead(int fd, void *buf, size_t size);
int safeOpen(const char *file, int oflag);
char readOneChar(int fd);
char *readLine(int fd, int line);
int getNumberOfLine(int fd);

/*init student functions*/
void initStudents(student students[], int fd, int n, pthread_t tids[]);
void seperateLine(char *line, student *std);

/*Messages to be printed on screen*/
void gNewHwMsg(char hw,double tl);
void gNoHwMsg();
void gNoMoneyMsg();
void mainPrintStudents(student students[],int n);
void mainNoHwMsg();
void mainNoMoneyMsg();
void mainReportMsg(student students[],int n,double leftMoney);
void stdWaitMsg(char *name);
void stdSolvingMsg(char *name,double price, double leftMoney,char hw);

#endif