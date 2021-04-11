#include "helper.h"

/*çocuk processleri line number bulmak için çocuk id - parent id yap defaultdaki suspend sil
* ve ödevi bir daha oku ona göre sinyallari tekrar ayarla sequential yapma!!!!
* bunun için global + handler metodunu kullnabilirsin.
*/
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

void checkArgument(int argc)
{
    if (argc < 2)
    {
        showUsageAndExit();
    }
}
void myPrintf(const char *str)
{
    ssize_t size = strlen(str);
    if (size != write(STDOUT_FILENO, str, size))
    {
        myStderr("write system call error\n");
    }
}
void myStderr(const char *str)
{
    ssize_t size = strlen(str);
    if (size != write(STDERR_FILENO, str, size))
    {
        perror("write system call error! ");
        exit(EXIT_FAILURE);
    }
}
void showUsageAndExit()
{
    myStderr("Usage: ./processM pathToFile\n");
    exit(EXIT_FAILURE);
}
int safeRead(int fd, void *buf, size_t size)
{
    int rd = read(fd, buf, size);
    if (rd == -1)
    {
        myStderr("reading error!\n");
        exit(EXIT_FAILURE);
    }
    return rd;
}
int safeWrite(int fd, void *buf, size_t size)
{
    int wrt = write(fd, buf, size);
    if (wrt == -1)
    {
        myStderr("writing error!\n");
        exit(EXIT_FAILURE);
    }
    return wrt;
}
int safeOpen(const char *file, int oflag)
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    int fd = open(file, oflag, mode);
    if (fd < 0)
    {
        myStderr("open error\n");
        exit(EXIT_FAILURE);
    }
    return fd;
}
int safeLseek(int fd, int offset, int whence)
{
    int pos = lseek(fd, offset, whence);
    if (pos == -1)
    {
        myStderr("lseek error\n");
        exit(EXIT_FAILURE);
    }
    return pos;
}
void lockFile(int fd){
    struct flock lock;
    memset(&lock, '\0', sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        myStderr("lock file error!\n");
        exit(EXIT_FAILURE);
    }
}

void unlockFile(int fd){
   struct flock lock;
    memset(&lock, '\0', sizeof(lock));
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        myStderr("unlock file error!\n");
        exit(EXIT_FAILURE);
    }
}
void writeEndofLine(int fd, double number, int line,char *buf)
{
    char c1;
    safeLseek(fd, 1, SEEK_SET);
    for (int i = 0; i < line; i++)
    {
        safeRead(fd, &c1, 1);
        while (c1 != '\n')
        {
            safeRead(fd, &c1, 1);
        }
    }
    int position = safeLseek(fd, -1, SEEK_CUR);
    char buffer[30];
    int n = sprintf(buffer, ",%.1f\n", number);
    struct stat fileStat;
    fstat(fd, &fileStat);
    int fileSize = (int)fileStat.st_size;
    ftruncate(fd, fileSize+n-1);
    pwrite(fd,buffer,n,position);
    pwrite(fd, &buf[position+1], fileSize-position-1,position+n);
}

void testLagrange(char *buff, int count)
{
    char *pt;
    pt = strtok(buff, ",");
    int counter = 3;
    double x[count], y[count], xi;
    int i = 0;
    double temp;
    const int citself = count;
    while (pt != NULL)
    {
        double a = atof(pt);
        if (i > count + 1)
        {
            break;
        }
        if (i == count)
        {
            xi = a;
            i++;
        }
        if (i == count + 1)
        {
            i++;
        }
        if (counter % 2 == 0 && i < citself)
        {
            x[i] = temp;
            y[i] = a;
            i++;
        }
        pt = strtok(NULL, ",");
        temp = a;
        counter++;
    }
    printf("res: %.1f\n", calculateInterpolation(x, y, xi, count));
}
char* readFile(int fd){
    int offset = 0;
    int bytes_read;
    int capacity = 0;
    int i = 0;
    char c;
    char *buffer = (char *)calloc(100, sizeof(char));    
    do{
        bytes_read = safeRead(fd, &c, 1); 
        offset += bytes_read;
        if (capacity <= offset + 1)
        {
            capacity = capacity + 100;
            buffer = realloc(buffer, capacity * sizeof(char));
        }
        buffer[i] = c;
        i++;
        
    } while (bytes_read == 1);
    return buffer;
}
void readLine(int fd,char *buff,int lineNumber,int numberOfCoor,double x[],double y[],double *xi){
    char c1;
    safeLseek(fd, 1, SEEK_SET);
    for (int i = 0; i < lineNumber-1; i++)
    {
        safeRead(fd, &c1, 1);
        while (c1 != '\n')
        {
            safeRead(fd, &c1, 1);
        }
    }
    int position;
    if (lineNumber == 1)
        position = safeLseek(fd, -1, SEEK_CUR);
    else
        position = safeLseek(fd, 0, SEEK_CUR);
    char *pt = strtok(&buff[position], ",");
    int counter = 3; //to keep track of two coordinate
    int i = 0;
    double temp;
    while (pt != NULL)
    {
        double a = atof(pt);
        if (i > numberOfCoor + 1)
        {
            break;
        }
        if (i == numberOfCoor)
        {
            *xi = a;
            i++;
        }
        if (i == numberOfCoor + 1)
        {
            i++;
        }
        if (counter % 2 == 0 && i < numberOfCoor)
        {
            x[i] = temp;
            y[i] = a;
            i++;
        }
        pt = strtok(NULL, ",");
        temp = a;
        counter++;
    }
    

}
//Ctrl+K+C, you'll comment out
//Ctrl+K+U will uncomment the code.
// int main(int argc, char *argv[])
// {
//     int offset = 0;
//     int bytes_read;
//     int capacity = 0;
//     checkArgument(argc);
//     char *path = argv[1];
//     int i = 0;
//     char c;
//     int fd = safeOpen(path, O_RDWR);
//    /* do
//     {
        
//         bytes_read = safeRead(fd, &c, 1); 
//         offset += bytes_read;
//         if (capacity <= offset + 1)
//         {
//             capacity = capacity + 100;
//             buffer = realloc(buffer, capacity * sizeof(char));
//         }
//         buffer[i] = c;
//         i++;
        
//     } while (bytes_read == 1); */
//     /*i = 0;
//     while (read(fd, &c, 1) == 1)
//     {
//         buffer[i] = c;
//         i++;
//         /* end of line
//         if (c == '\n')
//         {
//             buffer[i] = 0;
//             if (!strncmp(buffer, w, strlen(w)))
//             {
//                 printf("w was found in line %d\n", line);
//                 puts(buffer);
//                 n++;
//             }
//             line++;
//             i = 0;
//             continue;
//         }
//         i++; *
//     }*/
//     /*char *pt;
//     pt = strtok (buffer,",");
//     while (pt != NULL) {
//         double a = atof(pt);
//         printf("%.1f\n", a);
//         pt = strtok (NULL, ",");
//     }*/
//     //printf("%s\n",buffer);
//     //testLagrange(buffer,4);
//     /* All done.*/
//     //close(fd);
//     //int fd2=safeOpen(path, O_RDWR | O_APPEND);
//     //writeEndofLine(fd2,8.0,1);

//     //free(buffer);
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
//             char *buffer = (char *)calloc(100, sizeof(char));    
//             printf("Child %d (PID=%ld) start to working round 1...\n", j, (long)getpid());
//             do
//             {

//                 bytes_read = safeRead(fd, &c, 1);
//                 offset += bytes_read;
//                 if (capacity <= offset + 1)
//                 {
//                     capacity = capacity + 100;
//                     buffer = realloc(buffer, capacity * sizeof(char));
//                 }
//                 buffer[i] = c;
//                 i++;

//             } while (bytes_read == 1);
//             //printf("Child %d (PID=%ld)\n%s\n",j, (long)getpid(),buffer);
//             //testLagrange(buffer,4);
//             safeLseek(fd, 1, SEEK_SET);
//             safeLseek(fd, -1, SEEK_CUR);
//             printf("Child %d (PID=%ld) suspending...\n", j, (long)getpid());
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
//             writeEndofLine(fd,15.0,j,buffer);
//             free(buffer);
//             exit(EXIT_SUCCESS);
//         }
//         else
//         {
//             /*Parent Process*/
//             arr[j - 1] = pid;
//             sigemptyset(&emptyMask2);
//             if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
//                 errExit("sigsuspend");
//             //printf("signal to the... (PID=%ld)\n", (long)pid);
//             //kill(pid,SIGUSR1);
//             //printf("j: %d\n",j);
//             //counter--;
//         }
//     }
//     /*END OF FOR LOOP
//     sigemptyset(&emptyMask2);
//     if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
//         errExit("sigsuspend");*/
//     printf("Parent[%ld] has finished its waitng for round1..\n", (long)getpid());
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
