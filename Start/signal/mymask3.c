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

/*
* Tüm çocuklara sinyal göndermek için ID'leri bir arrayde tut ve sigchild'da 
* onları kullan burdan devam ett.
*
*/
#define SYNC_SIG SIGUSR1 /* Synchronization signal */
static volatile int numLive = 0;
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
    numLive++;
    if (numLive == 2)
    {
        printf("last child!!!\n");
    }
    printf("I am HEREEEE!!!\n");
    errno = savedErrno;
}
static void handler(int sig)
{
    //signal(sig, SIG_DFL); 
    //raise(sig);
}
int main(int argc, char *argv[])
{
    int j, sigCnt;
    sigset_t blockMask, emptyMask, blockMask2, emptyMask2, origMask;
    struct sigaction sa, sa2;

    setbuf(stdout, NULL); /* Disable buffering of stdout */

    sigCnt = 0;
    numLive = 2;

    sigemptyset(&sa.sa_mask);
    sigemptyset(&blockMask2);
    sigemptyset(&emptyMask2);
    sigaddset(&blockMask2, SYNC_SIG); /* Block signal */
    if (sigprocmask(SIG_BLOCK, &blockMask2, &origMask) == -1)
        errExit("sigprocmask");

    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = SA_RESTART;
    sa2.sa_handler = &handler;
    if (sigaction(SYNC_SIG, &sa2, NULL) == -1)
        errExit("sigaction_siguser");

    sa.sa_flags = 0;
    sa.sa_handler = &sigchldHandler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction_sigchild");

    /* Block SIGCHLD to prevent its delivery if a child terminates
       before the parent commences the sigsuspend() loop below */

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCHLD);
    if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1)
        errExit("sigprocmask");

    /* Create one child process for each command-line argument */

    for (j = 1; j < 9; j++)
    {
        switch (fork())
        {
        case -1:
            errExit("fork");

        case 0: /* Child - sleeps and then exits */
            printf("Child %d (PID=%ld) signaling and exiting...\n", j, (long)getpid());
            if (j == 8)
            {
                if (kill(getppid(), SIGCHLD) == -1)
                    errExit("kill");
            }
            else{
                if (kill(getppid(), SYNC_SIG) == -1)
                    errExit("kill");
            }
            /*if (kill(getppid(), SYNC_SIG) == -1)
                errExit("kill");*/
            //_exit(EXIT_SUCCESS);

        default: /* Parent - loops to create next child */
            printf("[%ld] Parent %d about to wait for signal\n", (long)getpid(), j);
            sigemptyset(&emptyMask2);
            if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
                errExit("sigsuspend_siguser");
            printf("[%ld] Parent %d got signal\n", (long)getpid(), j);

            /* If required, return signal mask to its original state */

            if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1)
                errExit("sigprocmask_siguser");

            /* Parent carries on to do other things... */

            break;
        }
    }

    /*sigemptyset(&emptyMask);
    while (numLiveChildren > 0)
    {
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend");
        sigCnt++;
    }*/

    printf("All children have terminated; SIGCHLD was caught "
           "%d times\n",
           numLive);

    return 0;
}
