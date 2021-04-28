#ifndef HELPER_H
#define HELPER_H
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
extern volatile __sig_atomic_t exitSignal;

typedef struct vaccine
{
    int vac1;
    int vac2;
}buffer;
typedef struct arg
{
    
   int nArg;
   int vArg;
   int cArg;
   int bArg;
   int tArg;
   char iArg[41];

}args;
void checkArguments(int argc, char **argv, args *givenArgs);
void showUsageAndExit();
void exitHandler(int signal);
void errExit(char *msg);
int safeLseek(int fd, int offset, int whence);
int safeRead(int fd, void *buf, size_t size);
int safeOpen(const char *file, int oflag);
char readOneChar(int fd);
#endif