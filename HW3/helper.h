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
    char msg[50];
};
struct receiver
{
    pid_t rec_pid;
    pid_t send_pid;
    char msg[50];
};
typedef struct st
{
    int sFlag;
    int fFlag;
    int bFlag;
    int mFlag;
    char *sArg;
    char *fArg;
    char *bArg;
    char *mArg;
    int count;

} args;
void checkArguments(int argc, char **argv, args *givenArgs);
void showUsageAndExit();
int getRandom(int n);
void exitHandler(int signal);