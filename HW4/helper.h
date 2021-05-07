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
    int index;
    int isBusy;
    pthread_t id;
} student;

void checkArguments(int argc, char **argv, args *givenArgs);
void showUsageAndExit();
void errExit(char *msg);
int safeLseek(int fd, int offset, int whence);
int safeRead(int fd, void *buf, size_t size);
int safeOpen(const char *file, int oflag);
char readOneChar(int fd);
char *readLine(int fd, int line);
int getNumberOfLine(int fd);
void initStudents(student students[], int fd, pthread_t tids[]);
void seperateLine(char *line, student *std);

#endif