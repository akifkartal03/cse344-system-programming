#include "helper.h"
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
void lockFile(int fd, struct flock *fl, int read);
void unlockFile(int fd, struct flock *fl);
void writeEndofLine(int fd, double number, int line)
{
    char c;
    safeLseek(fd, 1, SEEK_SET);
    for (int i = 0; i < line; i++)
    {
        safeRead(fd, &c, 1);
        while (c != '\n')
        {
            safeLseek(fd, 1, SEEK_CUR);
            safeRead(fd, &c, 1);
        }
    }
    safeLseek(fd, -1, SEEK_CUR);
    char buffer[30];
    int n = sprintf(buffer, ",%.1f,%.1f\n", number, number);
    printf("n:%d\n%s\n", n, buffer);
    int a = safeWrite(fd, buffer, n);
    printf("a:%d\n", a);
}

double calculateInterpolation(double x[], double y[], int xCount, int count)
{
    double yResult = 0;
    for (int i = 0; i < count; i++)
    {
        double nextY = y[i];
        for (int j = 0; j < count; j++)
        {
            if (j != i)
                nextY = nextY * (xCount - x[j]) / (x[i] - x[j]);
        }
        yResult = yResult + nextY;
    }
    return yResult;
}
void testLagrange(char *buff, int count)
{
    char *pt;
    pt = strtok(buff, ",");
    int counter = 3;
    double x[count], y[count], xi, yi;
    int i = 0;
    double temp;
    const int cplus1 = count + 1;
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
            yi = a;
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
    printf("res: %.1f\n", calculateInterpolation(x, y, xi, 4));
}
int main(int argc, char *argv[])
{
    int offset = 0;
    int bytes_read;
    int capacity = 0;
    checkArgument(argc);
    char *path = argv[1];
    int i = 0;
    char c;
    int fd = safeOpen(path, O_RDONLY);
    char *buffer = (char *)calloc(100, sizeof(char));
    /* Read from the file, one chunk at a time. Continue until read “comes up short”, that is, 
    reads less than we asked for. This indicates that we’ve hit the end of the file.
    do
    {
        
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
    /*i = 0;
    while (read(fd, &c, 1) == 1)
    {
        buffer[i] = c;
        i++;
        /* end of line
        if (c == '\n')
        {
            buffer[i] = 0;
            if (!strncmp(buffer, w, strlen(w)))
            {
                printf("w was found in line %d\n", line);
                puts(buffer);
                n++;
            }
            line++;
            i = 0;
            continue;
        }
        i++; *
    }*/
    /*char *pt;
    pt = strtok (buffer,",");
    while (pt != NULL) {
        double a = atof(pt);
        printf("%.1f\n", a);
        pt = strtok (NULL, ",");
    }
    testLagrange(buffer,4);
    /* All done.*/
    //close(fd);
    //int fd2=safeOpen(path, O_RDWR | O_APPEND);
    //writeEndofLine(fd2,8.0,1);

    //free(buffer);
    pid_t pid;
    int j, sigCnt;
    sigset_t blockMask, emptyMask, blockMask2, emptyMask2;
    struct sigaction sa, sa2;

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
            /*wait first signal*/
            sigset_t emptyMask3;
            sigemptyset(&emptyMask3);
            if (sigsuspend(&emptyMask3) == -1 && errno != EINTR)
                errExit("sigsuspend");

            do
            {

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
            testLagrange(buffer,4);
            //close(fd);
            free(buffer);
            printf("Child %d (PID=%ld) signaling to parent...\n", j, (long)getpid());
            /*Signal to the parent*/
            if (j == 8)
            {
                kill(getppid(), SIGUSR1);
            }
            /*wait second signal*/
            sigemptyset(&emptyMask3);
            if (sigsuspend(&emptyMask3) == -1 && errno != EINTR)
                errExit("sigsuspend");

            printf("Child (PID=%ld) GOT the signal...\n", (long)getpid());
            _exit(EXIT_SUCCESS);
        }
        else
        {
            /*Parent Process*/
            arr[j - 1] = pid;
            printf("signal to the... (PID=%ld)\n", (long)pid);
            kill(pid,SIGUSR1);
            //printf("j: %d\n",j);
            //counter--;
        }
    }
    /*END OF FOR LOOP*/
    sigemptyset(&emptyMask2);
    if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
        errExit("sigsuspend");
    printf("Parent[%ld] has finished its waitng for round1..\n", (long)getpid());
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
