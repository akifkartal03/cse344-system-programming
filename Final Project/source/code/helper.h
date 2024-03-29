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
    int port;
    int logFd;
    int poolSize;
    int datasetFd;
} args;

extern int LogFD;

/*Generic functions coming from through all hwks*/
void checkArguments(int argc, char **argv, args *givenArgs);
void showUsageAndExit();
void errExit(char *msg,int toLog);
int safeLseek(int fd, int offset, int whence,int toLog);
int safeRead(int fd, void *buf, size_t size,int toLog);
int safeWrite(int fd, void* buf, size_t size,int toLog);
int safeOpen(const char *file, int oflag,int toLog);
char *getTime();
void milSleep(int milSecond);

#endif