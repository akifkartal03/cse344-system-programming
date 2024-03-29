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
struct sender{
    pid_t pid;
    char fifo_name[50];
};
typedef struct potato
{
    pid_t pot_pid;
    int switches;
    char fifo_name[50];
    char real_name[30];
    int done;
    int is_opened;
}player;
struct args
{
    int sFlag;
    int fFlag;
    int bFlag;
    int mFlag;
    char sArg[35];
    char fArg[35];
    char bArg[35];
    char mArg[35];
    int count;

};
void checkArguments(int argc, char **argv, struct args *givenArgs);
void showUsageAndExit();
int getRandom(int n);
void exitHandler(int signal);
void errExit(char *msg);
int safeLseek(int fd, int offset, int whence);
int safeRead(int fd, void *buf, size_t size);
int safeOpen(const char *file, int oflag);
char *readLine(int fd,int line);
int getNumberOfLine(int fd);

#endif