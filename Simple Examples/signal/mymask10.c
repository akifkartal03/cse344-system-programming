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

static volatile int numLiveChildren = 0;
pid_t arr[8];
void handler(int signum){

}
void childHandler(int signum){

}
void errExit(char *msg)
{
    printf("%s error!\n", msg);
    exit(EXIT_FAILURE);
}
void sigchldHandler(int sig)
{
    int status, savedErrno;
    pid_t childPid;

    savedErrno = errno;

 
    /* Do nonblocking waits until no more dead children are found */

    while ((childPid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        numLiveChildren--;
    }

    if (childPid == -1 && errno != ECHILD)
        errExit("waitpid");
    errno = savedErrno;
}
int main(int argc, char *argv[])
{
    pid_t pid;
    int j, sigCnt;
    sigset_t blockMask, emptyMask,sigset;
    struct sigaction sa,sact;

    setbuf(stdout, NULL); /* Disable buffering of stdout */

    sigCnt = 0;
    numLiveChildren = 8;
    sigemptyset(&sact.sa_mask); 
    sact.sa_flags = 0;
    sact.sa_handler = &handler;
    if (sigaction(SIGUSR1, &sact, NULL) != 0)
        perror("1st sigaction() error");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = &sigchldHandler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction");

    /* Block SIGCHLD to prevent its delivery if a child terminates
       before the parent commences the sigsuspend() loop below */

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCHLD);
    if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1)
        errExit("sigprocmask");

    /* Create one child process for each command-line argument */

    for (j = 1; j < 9; j++)
    {
        pid = fork();
        if (pid == -1)
        {
            /* error */
            errExit("fork");
        }
        else if (pid == 0)
        {
            /* child */
            sigset_t sigset1;
            struct sigaction sact1;
            sigemptyset(&sact1.sa_mask); 
            sact1.sa_flags = 0;
            sact1.sa_handler = &childHandler;
            if (sigaction(SIGUSR1, &sact1, NULL) != 0)
                perror("1st sigaction() error");
            sigfillset(&sigset1); 
            sigdelset(&sigset1, SIGUSR1);
            if (sigsuspend(&sigset1) == -1 && errno != EINTR)
                perror("sigsuspend() returned -1 as expected");
            printf("Child %d (PID=%ld) signaling...\n", j, (long)getpid());
            kill(getppid(),SIGUSR1);
            sigfillset(&sigset1); 
            sigdelset(&sigset1, SIGUSR1);
            if (sigsuspend(&sigset1) == -1 && errno != EINTR)
                perror("sigsuspend() returned -1 as expected");
            printf("Child %d (PID=%ld) exiting...\n", j, (long)getpid());
            exit(EXIT_SUCCESS);
        }
        else{
            /* parent */
            arr[j - 1] = pid;
        }
    }
    sigfillset(&sigset); 
    sigdelset(&sigset, SIGUSR1);
    for (int i = 0; i < 8; i++)
    {
        kill(arr[i],SIGUSR1);
        if (sigsuspend(&sigset) == -1 && errno != EINTR) 
            perror("sigsuspend() returned -1 as expected");
    }
    for (int i = 0; i < 8; i++)
    {
        kill(arr[i],SIGUSR1);
    }
    sigemptyset(&emptyMask);
    while (numLiveChildren > 0)
    {
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend");
        sigCnt++;
    }

    printf("All children have terminated; SIGCHLD was caught "
           "%d times\n",
           sigCnt);

    exit(EXIT_SUCCESS);
}
