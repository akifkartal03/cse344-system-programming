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
static volatile int numLiveChildren = 0;
pid_t arr[8];
int counter = 0,indis = -1;
int roundQueue = 1 ,childQueue = 1;
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

    while ((childPid = waitpid(-1, &status,WNOHANG)) > 0)
    {
        numLiveChildren--;
    }

    if (childPid == -1 && errno != ECHILD)
        errExit("waitpid");
    errno = savedErrno;
}
static void handler(int sig)
{
    
     childQueue++;
}
static void handler2(int sig)
{
    //signal(sig, SIG_DFL); 
    //raise(sig);
     //printf("siguser1\n");
     counter++;
}
int main(int argc, char *argv[])
{
    pid_t pid;
    int j, sigCnt;
    sigset_t blockMask, emptyMask, blockMask2, emptyMask2, origMask,emptyMask3, origMask2;
    struct sigaction sa, sa2,sa3;

    setbuf(stdout, NULL); /* Disable buffering of stdout */

     sigCnt = 0;
    numLiveChildren = 8;

    sigemptyset(&sa.sa_mask);
    sigemptyset(&blockMask2);
    sigemptyset(&emptyMask2);
    sigemptyset(&emptyMask3);
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
        switch (pid = fork())
        {
        case -1:
            errExit("fork");

        case 0: /* Child - sleeps and then exits */
            sigemptyset(&sa3.sa_mask);
            sa3.sa_flags = SA_RESTART;
            sa3.sa_handler = &handler2;
            if (sigaction(SYNC_SIG, &sa3, NULL) == -1)
                errExit("sigaction_siguser");
            //printf("Child %d (PID=%ld) signaling and exiting...\n", childQueue, (long)getpid());
            if ( roundQueue ==1)
            {
                /*for (int i = 0; i < 8; i++)
                {
                    printf("id %d : %ld\n",i+1,(long)arr[i]);
                }*/
                /*İşini Yap*/
                roundQueue++;
                printf("Child %d (PID=%ld) signaling and exiting...\n", childQueue, (long)getpid());
                if (kill(getppid(), SYNC_SIG) == -1)
                    errExit("kill");
            }
            else{
                /*İşini Yap*/
                printf("exit\n");
                exit(0);
                
            }
            
            sigemptyset(&emptyMask2);
            if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
                errExit("sigsuspend_siguser");
            
                
            /*sigemptyset(&emptyMask2);
            if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
                errExit("sigsuspend_siguser");

            /* If required, return signal mask to its original state */

            /*if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1)
                errExit("sigprocmask_siguser");*/
            /*if (kill(getppid(), SYNC_SIG) == -1)
                errExit("kill");*/
            //_exit(EXIT_SUCCESS);

        default: /* Parent - loops to create next child */
            arr[j-1] = pid;
            if(roundQueue == 1){
                sigemptyset(&emptyMask2);
                if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
                    errExit("sigsuspend_siguser");
            }
            printf("[%ld] Parent %d got signal\n", (long)getpid(), j);
            if (childQueue == 8 && roundQueue == 1)
            {
                printf("1.error hesaplandı parent\n");
                //indis++;
                roundQueue = 2;
                childQueue = 1;
                /*if (kill(arr[indis], SYNC_SIG) == -1)
                    errExit("kill");*/
            }
            if (childQueue != 8 && roundQueue == 1)
            {
                //eempty
                printf("burada1\n");
               
            }
           
            
            /* If required, return signal mask to its original state */

            if (roundQueue == 1)
            {
                if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1)
                    errExit("sigprocmask_siguser");
            }
            

            /*if (roundQueue == 2)
            {
                indis++;
                if (kill(arr[indis], SYNC_SIG) == -1)
                    errExit("kill");
            }*/
            

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
    /*if (counter == 7 && roundQueue == 1)
    {
        roundQueue = 2;
        counter = 0;
        if (kill(arr[0], SYNC_SIG) == -1)
            errExit("kill");
    }
    //roundQueue = 2;
    counter = 0;*/
     /*if (roundQueue == 2 && childQueue!=8) 
    {
        
        indis++;
        printf("burada2\n");
        printf("pid : %ld\n",(long)arr[indis]);
        if (kill(arr[indis], SYNC_SIG) == -1)
            errExit("kill");
    }*/
    sigemptyset(&emptyMask);
    while (numLiveChildren > 0)
    {
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend");
        sigCnt++;
    }
    printf("All children have terminated; SIGCHLD was caught "
           "%d times\n",
           numLiveChildren);

    return 0;
}
