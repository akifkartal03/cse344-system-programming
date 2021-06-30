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
void errExit(char *msg)
{
    printf("%s error!\n", msg);
    exit(EXIT_FAILURE);
}
void handler(int sig)
{
    //printf("sigHander: %d\n",counter);
    //counter--;
}
void handler2(int sig)
{
    //printf("sigHander: %d\n",counter);
    //counter--;
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
    sigset_t blockMask, emptyMask,blockMask2, emptyMask2;
    struct sigaction sa,sa2;

    setbuf(stdout, NULL); /* Disable buffering of stdout */

    sigCnt = 0;
    numLiveChildren = 8;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = &sigchldHandler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction");

    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;
    sa2.sa_handler = &handler;
    if (sigaction(SIGUSR1, &sa2, NULL) == -1)
        errExit("sigaction");
    /* Block SIGCHLD to prevent its delivery if a child terminates
       before the parent commences the sigsuspend() loop below */

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCHLD);
    if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1)
        errExit("sigprocmask");
    
    sigemptyset(&blockMask2);
    sigaddset(&blockMask2, SIGUSR1);
    if (sigprocmask(SIG_SETMASK, &blockMask2, NULL) == -1)
        errExit("sigprocmask");

    /* Create one child process for each command-line argument */

    for (j = 1; j < 9; j++)
    {
        pid = fork();
        if (pid == -1)
        {
            errExit("fork");
        }
        else if (pid == 0)
        {
            /* Child Processs */

            /*Wait for sigusr1 signal*/
            struct sigaction sa3;
            sigemptyset(&sa3.sa_mask);
            sa3.sa_flags = 0;
            sa3.sa_handler = &handler2;
            if (sigaction(SIGUSR1, &sa3, NULL) == -1)
                errExit("sigaction");
                
            printf("Child %d (PID=%ld) signaling to parent...\n", j, (long)getpid());
            /*Signal to the parent*/
            if (j == 8)
            {
                kill(getppid(), SIGUSR1);
            }
            /*wait second signal*/
            sigset_t emptyMask3;
            sigemptyset(&emptyMask3);
            if (sigsuspend(&emptyMask3) == -1 && errno != EINTR)
                errExit("sigsuspend");
    
            printf("Child (PID=%ld) GOT the signal...\n", (long)getpid());
            _exit(EXIT_SUCCESS);
        
        }
        else{
            /*Parent Process*/
            arr[j-1] = pid;
            //printf("j: %d\n",j);
            //counter--;
        }
        
    }
    /*END OF FOR LOOP*/
    sigemptyset(&emptyMask2);
    if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
        errExit("sigsuspend");
    printf("Parent[%ld] has finished its waitng for round1..\n",(long)getpid());
    //printf("counter: %d\n",counter);
    /*Calculate firs errrorr!!!*/
    for (int i = 0; i < 8; i++)
    {
        kill(arr[i], SIGUSR1);
    }
    /*SECOUNd*/
    sigemptyset(&emptyMask);
    while (numLiveChildren > 0)
    {
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend");
        sigCnt++;
    }
    printf("Parent (PID=%ld) exiting 2.error calculation\n", (long)getpid());
    printf("All children have terminated; SIGCHLD was caught "
           "%d times\n",
           sigCnt);

    return 0;
}
