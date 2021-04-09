#include "helper.h"


int main(int argc, char *argv[]){
    checkArgument(argc-1);
    int line = atoi(argv[2]);
    char *path = argv[1];
    int fd = safeOpen(path, O_RDWR);
    char *buff = readFile(fd);
    double x[6], y[6], xi;
    readLine(fd,buff,line,6,x,y,&xi);
    for (int i = 0; i < 6; i++)
    {
        printf("%.1f,",x[i]);
        printf("%.1f,",y[i]);
    }
    printf("\nxi:%.1f\n",xi);
    return 0;
}

// static volatile int numLiveChildren = 0;
// void errExit(char *msg)
// {
//     printf("%s error!\n", msg);
//     exit(EXIT_FAILURE);
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
//     int j, sigCnt;
//     sigset_t blockMask, emptyMask;
//     struct sigaction sa;

//     setbuf(stdout, NULL); /* Disable buffering of stdout */

//     sigCnt = 0;
//     numLiveChildren = 8;

//     sigemptyset(&sa.sa_mask);
//     sa.sa_flags = 0;
//     sa.sa_handler = &sigchldHandler;
//     if (sigaction(SIGCHLD, &sa, NULL) == -1)
//         errExit("sigaction");

//     /* Block SIGCHLD to prevent its delivery if a child terminates
//        before the parent commences the sigsuspend() loop below */

//     sigemptyset(&blockMask);
//     sigaddset(&blockMask, SIGCHLD);
//     if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1)
//         errExit("sigprocmask");

//     /* Create one child process for each command-line argument */

//     for (j = 1; j < 9; j++)
//     {
//         switch (fork())
//         {
//         case -1:
//             errExit("fork");

//         case 0: /* Child - sleeps and then exits */
//             printf("Child %d (PID=%ld) exiting...\n", j, (long)getpid());
//             _exit(EXIT_SUCCESS);

//         default: /* Parent - loops to create next child */
//             break;
//         }
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
