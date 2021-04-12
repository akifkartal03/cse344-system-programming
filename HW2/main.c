#include "helper.h"
static volatile int numLiveChildren = 0;
pid_t arr[8];
void handler(int signum){
    if (signum == SIGUSR1)
    {
        
    }
    
}
void childHandler(int signum){
    if (signum == SIGUSR1)
    {
        
    }
}
void errExit(char *msg)
{
    printf("%s error!\n", msg);
    exit(EXIT_FAILURE);
}
/*This part was taken from book(Listing 26-5).
* Reaping dead children via a handler for SIGCHLD
*/
void sigchldHandler(int sig)
{
    if (sig == SIGCHLD)
    {
        
    }
    int status, savedErrno;
    pid_t childPid;
    savedErrno = errno;
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
    /*Open File*/
    int fd = safeOpen(path, O_RDWR);

    /*Start to Create Process*/

    pid_t pid;
    int j, sigCnt;
    sigset_t blockMask, emptyMask,sigset;
    struct sigaction sa,sact;

    //setbuf(stdout, NULL);

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

    
    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCHLD);
    if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1)
        errExit("sigprocmask");

    
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
            struct flock l;
            memset(&l, '\0', sizeof(l));
            sigemptyset(&sact1.sa_mask); 
            sact1.sa_flags = 0;
            sact1.sa_handler = &childHandler;
            if (sigaction(SIGUSR1, &sact1, NULL) != 0)
                perror("1st sigaction() error");
            /*Wait Start Signal*/ 
            int fd1 = safeOpen(path, O_RDWR);   
            sigfillset(&sigset1); 
            sigdelset(&sigset1, SIGUSR1);
            if (sigsuspend(&sigset1) == -1 && errno != EINTR)
                errExit("suspend");
            /*calculate the Lagrange polynomial p of degree 5 using the 6 first coordinates of that row.*/    
            double x[row][column],y[row][column],x1[],y1[];
            readFile(x,y,0);
            readLine(x,y,x1,y1,j-1);
            double res = calculateInterpolation(x1,y1,x[j-1][7],6);
            lockFile(fd1);
            char *buf = readFile2(fd1);
            writeEndofLine(fd1,res,j,buf);
            unlockFile(fd1);
            free(buf);
            close(fd1);
            /*signal to the parent that you finished*/
            kill(getppid(),SIGUSR1);
            /*and wait a signal is coming*/
            sigfillset(&sigset1); 
            sigdelset(&sigset1, SIGUSR1);
            if (sigsuspend(&sigset1) == -1 && errno != EINTR)
                errExit("suspend");

            fd1 = safeOpen(path, O_RDWR);
            lockFile(fd1);
            buf = readFile2(fd1);
            res = calculateInterpolation(x1,y1,x[j-1][7],7);
            writeEndofLine(fd1,res,j,buf);
            unlockFile(fd1);
            /*print results*/
            double coofs[7];
            getCoefficents(x1,y1,coofs);
            printCoofs(coofs,j-1);
            free(buf);
            close(fd1);
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
    printf("Error of polynomial of degree 5: %.1f\n",round1_error(fd));
    fflush(stdout);
    for (int i = 0; i < 8; i++)
    {
        kill(arr[i],SIGUSR1);
    }
    sigfillset(&emptyMask); 
    sigdelset(&emptyMask, SIGCHLD);
    while (numLiveChildren > 0)
    {
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend");
        sigCnt++;
    }
    printf("Error of polynomial of degree 6: %.1f\n",round2_error(fd));
    fflush(stdout);
    close(fd);
    return 0;
}