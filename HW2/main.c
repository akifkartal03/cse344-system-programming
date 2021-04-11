#include "helper.h"
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
    checkArgument(argc);
    char *path = argv[1];
    int fd = safeOpen(path, O_RDWR | O_SYNC);
    /*Start to Create Process*/

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
            printf("Child %d (PID=%ld) start to work...\n", j, (long)getpid());
            sigset_t sigset1;
            struct sigaction sact1;
            sigemptyset(&sact1.sa_mask); 
            sact1.sa_flags = 0;
            sact1.sa_handler = &childHandler;
            if (sigaction(SIGUSR1, &sact1, NULL) != 0)
                perror("1st sigaction() error");
            /*Wait Start Signal*/    
            sigfillset(&sigset1); 
            sigdelset(&sigset1, SIGUSR1);
            if (sigsuspend(&sigset1) == -1 && errno != EINTR)
                errExit("suspend");
            /*calculate the Lagrange polynomial p of degree 5 using the 6 first coordinates of that row.*/    
            double x[6],y[6],xi;
            int line = getpid()-getppid();
            lockFile(fd);
            char *buf = readFile(fd);
            printf("%s\n",buf);
            readLine(fd,buf,line,6,x,y,&xi);
            double res = calculateInterpolation(x,y,xi,6);
            printf("res:%.1f\n",res);
            writeEndofLine(fd,res,line,buf);
            unlockFile(fd);
            printf("Child %d (PID=%ld) signaling...\n", j, (long)getpid());
            kill(getppid(),SIGUSR1);
            sigfillset(&sigset1); 
            sigdelset(&sigset1, SIGUSR1);
            if (sigsuspend(&sigset1) == -1 && errno != EINTR)
                errExit("suspend");
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
            errExit("suspend");
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
// static volatile int numLiveChildren = 0;
// pid_t arr[8];
// void errExit(char *msg)
// {
//     printf("%s error!\n", msg);
//     exit(EXIT_FAILURE);
// }
// void handler(int sig)
// {
//     //printf("sigHander: %d\n",counter);
//     //counter--;
// }
// void handler2(int sig)
// {
//     //printf("sigHander: %d\n",counter);
//     //counter--;
// }
// void sigchldHandler(int sig)
// {
//     int status, savedErrno;
//     pid_t childPid;

//     savedErrno = errno;

//     /* Do nonblocking waits until no more dead children are found */

//     while ((childPid = waitpid(-1, &status, WNOHANG)) > 0)
//     {
//         numLiveChildren--;
//     }

//     if (childPid == -1 && errno != ECHILD)
//         errExit("waitpid");
//     errno = savedErrno;
// }

// int main(int argc, char *argv[])
// {
    
//     checkArgument(argc);
//     char *path = argv[1];
//     int fd = safeOpen(path, O_RDWR);
//    /*create m children*/
//     pid_t pid;
//     int j, sigCnt;
//     sigset_t blockMask, emptyMask, blockMask2, emptyMask2,blockMask3, emptyMask5;
//     struct sigaction sa, sa2,sa4;

//     setbuf(stdout, NULL); 

//     sigCnt = 0;
//     numLiveChildren = 8;

//     sigemptyset(&sa.sa_mask);
//     sa.sa_flags = 0;
//     sa.sa_handler = &sigchldHandler;
//     if (sigaction(SIGCHLD, &sa, NULL) == -1)
//         errExit("sigaction");

//     sigemptyset(&sa2.sa_mask);
//     sa2.sa_flags = 0;
//     sa2.sa_handler = &handler;
//     if (sigaction(SIGUSR1, &sa2, NULL) == -1)
//         errExit("sigaction");
    
//     sigemptyset(&sa4.sa_mask);
//     sa4.sa_flags = 0;
//     sa4.sa_handler = &handler;
//     if (sigaction(SIGUSR2, &sa4, NULL) == -1)
//         errExit("sigaction");

//     sigemptyset(&blockMask);
//     sigaddset(&blockMask, SIGCHLD);
//     if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1)
//         errExit("sigprocmask");

//     sigemptyset(&blockMask2);
//     sigaddset(&blockMask2, SIGUSR1);
//     if (sigprocmask(SIG_SETMASK, &blockMask2, NULL) == -1)
//         errExit("sigprocmask");

    
//     sigemptyset(&blockMask3);
//     sigaddset(&blockMask3, SIGUSR2);
//     if (sigprocmask(SIG_SETMASK, &blockMask3, NULL) == -1)
//         errExit("sigprocmask");

//     for (j = 1; j < 9; j++)
//     {
//         pid = fork();
//         if (pid == -1)
//         {
//             errExit("fork");
//         }
//         else if (pid == 0)
//         {
           
//             struct sigaction sa3;
//             sigemptyset(&sa3.sa_mask);
//             sa3.sa_flags = 0;
//             sa3.sa_handler = &handler2;
//             if (sigaction(SIGUSR1, &sa3, NULL) == -1)
//                 errExit("sigaction");
//             /*wait first signal*/
            
//             /*sigemptyset(&emptyMask3);
//             if (sigsuspend(&emptyMask3) == -1 && errno != EINTR)
//                 errExit("sigsuspend");*/
//             double x[6],y[6],xi;
//             int line = getpid()-getppid();
//             char *buf = readFile(fd);
//             printf("%s\n",buf);
//             readLine(fd,buf,line,6,x,y,&xi);
//             double res = calculateInterpolation(x,y,xi,6);
//             printf("res:%.1f\n",res);
//             writeEndofLine(fd,res,line,buf);

//             kill(getppid(), SIGUSR1);
//             /*Signal to the parent*/
//             /*if (j == 8)
//             {
//                 printf("Child %d (PID=%ld) signaling to parent...\n", j, (long)getpid());
//                 kill(getppid(), SIGUSR1);
//             }*/
//             /*wait second signal*/
//             sigset_t emptyMask3;
//             sigemptyset(&emptyMask3);
//             if (sigsuspend(&emptyMask3) == -1 && errno != EINTR)
//                 errExit("sigsuspend");

//             printf("Child (PID=%ld) GOT the signal...\n", (long)getpid());
//             //writeEndofLine(fd,15.0,j,buffer);
//             //free(buffer);
//             exit(EXIT_SUCCESS);
//         }
//         else
//         {
//             /*Parent Process*/
//             arr[j - 1] = pid;
//             sigemptyset(&emptyMask2);
//             if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
//                 errExit("sigsuspend");
//             printf("maske gitti\n");
//             //kill(pid,SIGUSR1);
//             //printf("j: %d\n",j);
//             //counter--;
//         }
//     }
//     /*END OF FOR LOOP
//     sigemptyset(&emptyMask2);
//     if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
//         errExit("sigsuspend");*/
//     printf("Parent[%ld] has finished its waiting for round1..\n", (long)getpid());
//     //printf("counter: %d\n",counter);
//     /*Calculate firs errrorr!!!*/
//     for (int i = 0; i < 8; i++)
//     {
//         printf("Child[%ld]\n", (long)arr[i]);
//         kill(arr[i], SIGUSR1);
//         sigemptyset(&emptyMask2);
//         if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
//             errExit("sigsuspend");
//     }
//     /*SECOUNd*/
//     sigemptyset(&emptyMask);
//     while (numLiveChildren > 0)
//     {
//         if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
//             errExit("sigsuspend");
//         sigCnt++;
//     }
//     printf("Parent (PID=%ld) exiting 2.error calculation\n", (long)getpid());
//     printf("All children have terminated; SIGCHLD was caught "
//            "%d times\n",
//            sigCnt);
//     close(fd);
//     return 0;
// }







// /*int main(int argc, char *argv[]){
//     checkArgument(argc-1);
//     //int line = atoi(argv[2]);
//     char *path = argv[1];
//     int fd = safeOpen(path, O_RDWR);
//     char *buff = readFile(fd);
//     //double x[6], y[6], xi;
//     //readLine(fd,buff,line,6,x,y,&xi);
//     //printf("\nxi:%.1f\n",xi);
//     //writeEndofLine(fd,calculateInterpolation(x,y,xi,6),2,buff);
//     testLagrange(buff,6);
//     return 0;
// }*/

// static volatile int numLiveChildren = 0;
// int finished = 0;
// siginfo_t si;
// sigset_t allSigs;
// pid_t arr[8];
// void errExit(char *msg)
// {
//     printf("%s error!\n", msg);
//     exit(EXIT_FAILURE);
// }

// static void siginfoHandler(int sig, siginfo_t *si, void *ucontext){

//     finished--;
//     printf("caught signal %d\n", finished);
//     sigwaitinfo(&allSigs,NULL);
// }
// void handler2(int signo){
    
//     if (signo == SIGUSR1)
//     {
//         finished--;
//         printf("handler2: %d\n", finished);
//     }
// }
// void handler3(int signo){
    
//     if (signo == SIGUSR2)
//     {
//         finished--;
//         printf("handler3: %d\n", finished);
//     }
    
            
// }
// void sigchldHandler(int sig)
// {
//     if (sig == SIGCHLD)
//     {
//         int status, savedErrno;
//         pid_t childPid;

//         savedErrno = errno;


//         /* Do nonblocking waits until no more dead children are found */

//         while ((childPid = waitpid(-1, &status, WNOHANG | WUNTRACED)) >= 0)
//         {
//             numLiveChildren--;
//         }
//         printf("hand:%d\n",numLiveChildren);
//         if (childPid == -1 && errno != ECHILD)
//             errExit("waitpid");
//         errno = savedErrno;
//     }
// }
// int main(int argc, char *argv[])
// {
//     checkArgument(argc);
//     /*open file*/
//     char *path = argv[1];
//     int fd = safeOpen(path, O_RDWR);
//     pid_t childPid;
//     pid_t parentPid = getpid();
//     printf("%d\n",fd);
//     printf("%ld\n",(long)parentPid);
//     int j, sigCnt;
//     sigset_t blockMask, emptyMask;
//     struct sigaction sa,sa2,sa4;
//     setbuf(stdout, NULL); /* Disable buffering of stdout */
//     sigemptyset(&allSigs);
//     sigaddset(&allSigs,SIGUSR1);
//     sigCnt = 0;
//     numLiveChildren = 8;
//     finished = 8;
//     sigemptyset(&sa.sa_mask);
//     sa.sa_flags = 0;
//     sa.sa_handler = &sigchldHandler;
//     if (sigaction(SIGCHLD, &sa, NULL) == -1)
//         errExit("sigaction");

//     sigemptyset(&sa2.sa_mask);
//     sa2.sa_flags = SA_SIGINFO;
//     sa2.sa_sigaction = siginfoHandler;
//     if (sigaction(SIGUSR1, &sa2, NULL) == -1)
//         errExit("sigaction");

//     sigemptyset(&sa4.sa_mask);
//     sa4.sa_flags = 0;
//     sa4.sa_handler = &handler3;
//     if (sigaction(SIGUSR2, &sa4, NULL) == -1)
//         errExit("sigaction");

//     /* Block SIGCHLD to prevent its delivery if a child terminates
//        before the parent commences the sigsuspend() loop below */

//     sigemptyset(&blockMask);
//     sigaddset(&blockMask, SIGCHLD);
//     //sigaddset(&blockMask, SIGUSR1);
//     if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1)
//         errExit("sigprocmask");

//     union sigval sv;
//     /* Create one child process for each command-line argument */

//     for (j = 1; j < 9; j++)
//     {
//         childPid = fork();
//         if (childPid == -1)
//         {
//             errExit("fork");
//         }
//         else if (childPid == 0)
//         {
//             /* Child Process */

//             /*Wait for sigusr1 signal*/
//             struct sigaction sa3;
//             sigemptyset(&sa3.sa_mask);
//             sa3.sa_flags = 0;
//             sa3.sa_handler = &handler2;
//             if (sigaction(SIGUSR1, &sa3, NULL) == -1)
//                 errExit("sigaction");
//             printf("Child %d (PID=%ld) signaling to parent...\n", j, (long)getpid());    
//             sigqueue(getppid(),SIGUSR1,sv);
//             /*wait second signal*/
//             sigset_t emptyMask3;
//             sigemptyset(&emptyMask3);
//             if (sigsuspend(&emptyMask3) == -1 && errno != EINTR)
//                 errExit("sigsuspend");    
//             printf("Child %d (PID=%ld) exiting...\n", j, (long)getpid());
//             exit(EXIT_SUCCESS);
//         }
//         else
//         {
//             arr[j-1] = childPid;
            
//         }
        
        
        
//     }
//     sigwaitinfo(&allSigs, &si);
//     sigemptyset(&emptyMask);
//     while (finished > 0)
//     {
//         if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
//             errExit("sigsuspend"); 
//     }
//     printf("Parent finished round1...\n");
//     for (int k = 0; k < 8; k++)
//     {
//         kill(arr[k], SIGUSR1);
//     }
    
//     sigemptyset(&emptyMask);
//     while (numLiveChildren > 0)
//     {
//         if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
//             errExit("sigsuspend");
//         sigCnt++;
//     }

//     printf("All children have terminated; SIGCHLD was caught "
//            "%d times\n",
//            sigCnt);

//     exit(EXIT_SUCCESS);
// }
