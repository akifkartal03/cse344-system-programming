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
#define SYNC_SIG SIGUSR1 /* Synchronization signal */

static void handler(int sig)
{
    //empty
}
void errExit(char *msg)
{
    printf("%s error!\n", msg);
    exit(EXIT_FAILURE);
}
int main(int argc, char *argv[])
{
    pid_t childPid;
    sigset_t blockMask, origMask, emptyMask;
    struct sigaction sa;

    setbuf(stdout, NULL); /* Disable buffering of stdout */

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SYNC_SIG); /* Block signal */
    if (sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1)
        errExit("sigprocmask");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = &handler;
    if (sigaction(SYNC_SIG, &sa, NULL) == -1)
        errExit("sigaction");

    for (int j = 1; j < 9; j++)
    {
        switch (childPid = fork())
        {
        case -1:
            errExit("fork");

        case 0: /* Child */

            /* Child does some required action here... */

            printf("[%ld] Child %d started - doing some work\n", (long)getpid(),j);

            printf("[%ld] Child %d about to signal parent\n", (long)getpid(),j);
            if (kill(getppid(), SYNC_SIG) == -1)
                errExit("kill");

            /* Now child can do other things... */

            //_exit(EXIT_SUCCESS);

        default: /* Parent */

            /* Parent may do some work here, and then waits for child to
           complete the required action */

            printf("[%ld] Parent %d about to wait for signal\n", (long)getpid(),j);
            sigemptyset(&emptyMask);
            if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
                errExit("sigsuspend");
            printf("[%ld] Parent %d got signal\n", (long)getpid(),j);

            /* If required, return signal mask to its original state */

            if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1)
                errExit("sigprocmask");

            //wait(NULL);

            break;
        }
    }
    return 0;
}