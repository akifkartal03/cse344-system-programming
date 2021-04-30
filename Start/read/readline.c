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
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/mman.h>
static volatile int numLiveChildren = 0;
void errExit(char *msg)
{
    //In case of an arbitrary error,
    //exit by printing to stderr a nicely formatted informative message.
    fprintf(stderr, "%s:%s\n", msg, strerror(errno));
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
int safeRead(int fd, void *buf, size_t size)
{
    int rd = read(fd, buf, size);
    if (rd == -1)
    {
        errExit("reading error!");
    }
    return rd;
}
int safeOpen(const char *file, int oflag)
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    int fd = open(file, oflag, mode);
    if (fd < 0)
    {
        errExit("open error!");
    }
    return fd;
}
int safeLseek(int fd, int offset, int whence)
{
    int pos = lseek(fd, offset, whence);
    if (pos == -1)
    {
        errExit("lseek error!");
    }
    return pos;
}
int getMin(int a, int b){
    if (a < b)
    {
        return a;
    }
    else{
        return b;
    }
    
}
char *readLine(int fd, int line)
{
    char c1;
    safeLseek(fd, 0, SEEK_SET);
    for (int i = 0; i < line - 1; i++)
    {
        do
        {
            safeRead(fd, &c1, 1);
        } while (c1 != '\n');
    }
    int offset = 0;
    int bytes_read;
    int capacity = 0;
    int i = 0;
    char c;
    char *buffer = (char *)calloc(50, sizeof(char));
    do
    {
        bytes_read = safeRead(fd, &c, 1);
        offset += bytes_read;
        if (capacity <= offset + 1)
        {
            capacity = capacity + 20;
            buffer = realloc(buffer, capacity * sizeof(char));
        }
        buffer[i] = c;
        i++;

    } while (c != '\n');
    buffer[i - 1] = '\0';
    return buffer;
}
int getNumberOfLine(int fd)
{
    int bytes_read;
    int i = 0;
    char c;
    safeLseek(fd, 0, SEEK_SET);
    do
    {
        bytes_read = safeRead(fd, &c, 1);
        if (c == '\n')
        {
            i++;
        }
    } while (bytes_read == 1);
    return i - 1;
}
int getRandom(int n)
{
    //srand(time(0));
    return (rand() % n) + 1;
}
char readOneChar(int fd){
    char c;
    int eof;
    // x represent end of file
    eof = safeRead(fd, &c, 1);
    if (eof != 0)
    {
        if (c != '\n')
        {
            return c;
        }
        eof = safeRead(fd, &c, 1);
        if (eof != 0)
        {
            return c;
        }
        return 'x';
        
    }
    return 'x'; 
}
void remove_vaccine(char *buffer, int size, int index)
{
   for(int i = index; i < size - 1; i++){
       buffer[i] = buffer[i + 1];
   } 
}
sem_t *sem_mutex;
sem_t *sem_index;
sem_t *sem_full;
sem_t *sem_empty;
static char memoryName[50];
static char memoryName2[50];
static void removeAll(void)
{
    if (sem_close(sem_full) == -1)
        errExit("sem_close");
    if (sem_close(sem_empty) == -1)
        errExit("sem_close");
    if (sem_close(sem_mutex) == -1)
        errExit("sem_close");
    if (sem_close(sem_index) == -1)
        errExit("sem_close");
    sem_unlink("mutex");
    sem_unlink("index");
    sem_unlink("full");
    sem_unlink("empty");
    shm_unlink(memoryName);
    shm_unlink(memoryName2);
}
typedef struct test{

    int index1;
    int index2;
}data;
int main(int argc, char *argv[])
{
    int j, sigCnt;
    sigset_t blockMask, emptyMask;
    struct sigaction sa;
    data *data1;
    setbuf(stdout, NULL); /* Disable buffering of stdout */

    sigCnt = 0;
    numLiveChildren = 10;
    /*save remove method*/
    if (atexit(removeAll) != 0)
        errExit("atexit");
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
    struct stat sb;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    strcpy(memoryName,"clinic");
    int memFd = shm_open(memoryName, O_CREAT | O_RDWR, mode);
    if (memFd == -1)
        errExit("shm_open error!");
    if (fstat(memFd, &sb) == -1)
        errExit("fstat error");
    if (ftruncate(memFd, 10) == -1)  
        errExit("ftruncate error");
    strcpy(memoryName2,"indis");
    int mem2Fd = shm_open(memoryName2, O_CREAT | O_RDWR, mode);
    if (mem2Fd == -1)
        errExit("shm_open error!");
    if (fstat(mem2Fd, &sb) == -1)
        errExit("fstat error");
    if (ftruncate(mem2Fd, sizeof(*data1)) == -1)  
        errExit("ftruncate error");

    char *buffer = (char *)mmap(NULL, 10, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
    if (buffer == MAP_FAILED)
        errExit("mmap");
    for (int i = 0; i < 10; ++i) {
        buffer[i] = '0';
    }
    data *place = (data *)mmap(NULL, sizeof(*data1), PROT_READ | PROT_WRITE, MAP_SHARED, mem2Fd, 0);
    if (place == MAP_FAILED)
        errExit("mmap");
    place->index1 = 0;
    place->index2 = 0;


    sem_mutex = sem_open("mutex", O_CREAT, 0666, 1);
    if (sem_mutex == SEM_FAILED)
        errExit("sem_open error!");

    sem_index = sem_open("index", O_CREAT, 0666, 0);
    if (sem_index == SEM_FAILED)
        errExit("sem_open error!");

    sem_full = sem_open("full", O_CREAT, 0666, 0);
    if (sem_full == SEM_FAILED)
        errExit("sem_open error!");
    sem_empty = sem_open("empty", O_CREAT, 0666, 10);
    if (sem_empty == SEM_FAILED)
        errExit("sem_open error!");
    int fd = safeOpen(argv[1], O_RDWR);
    /* Create one child process for each command-line argument */
    int counter;
    int has1 = 0;
    int has2 = 0;
    for (j = 1; j < 8; j++)
    {
        switch (fork())
        {
        case -1:
            errExit("fork");

        case 0: /* Child - sleeps and then exits */

            
            if (sem_getvalue(sem_mutex, &counter) == -1)
                errExit("sem_get");
            printf("Producer %d (PID=%ld) running, mutex:%d\n", j, (long)getpid(),counter);
            char *nurse_buffer = (char *)mmap(NULL, 10, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
            if (nurse_buffer == MAP_FAILED)
                errExit("mmap");

            data *place2 = (data *)mmap(NULL, sizeof(*data1), PROT_READ | PROT_WRITE, MAP_SHARED, mem2Fd, 0);
            if (place2 == MAP_FAILED)
                errExit("mmap");

            if (sem_wait(sem_empty) == -1)
                    errExit("sem_wait");
            if (sem_wait(sem_mutex) == -1)
                errExit("sem_wait");


            char vaccine = readOneChar(fd);  
            if (vaccine == '1')
            {
                place2->index1 = place2->index1 + 1;
            }
            if (vaccine == '2')
            {
                place2->index2 = place2->index2 + 1;
            }
            printf("Vac1::%d\n",place2->index1);
            printf("Vac2::%d\n",place2->index2);
            if (place2->index1 > 0 && place2->index2 > 0)
            {
                int res;
                if (sem_getvalue(sem_full, &res) == -1)
                    errExit("sem_get");

                int min = getMin(place2->index1,place2->index2);

                if (min - res >= 1)
                {
                    printf("now POSTINGGG.....!!\n");
                    if (sem_post(sem_full) == -1){
                        errExit("sem_post"); 
                    }

                }
                
            }
        
            if (sem_post(sem_mutex) == -1)
                errExit("sem_post");
            printf("Producer %d (PID=%ld) exiting...\n", j, (long)getpid());
            _exit(EXIT_SUCCESS);

        default: 
        /* Parent - loops to create next child */
            break;
        }
    }
    int isFound1 = 0;
    int isFound2 = 0;
    for (j = 1; j < 4; j++)
    {
        switch (fork())
        {
        case -1:
            errExit("fork");

        case 0: /* Child - sleeps and then exits */

            
            if (sem_getvalue(sem_mutex, &counter) == -1)
                errExit("sem_get");
            printf("Consumer %d (PID=%ld) running, mutex:%d\n", j, (long)getpid(),counter);
            char *nurse_buffer = (char *)mmap(NULL, 10, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
            if (nurse_buffer == MAP_FAILED)
                errExit("mmap");

            data *place2 = (data *)mmap(NULL, sizeof(*data1), PROT_READ | PROT_WRITE, MAP_SHARED, mem2Fd, 0);
            if (place2 == MAP_FAILED)
                errExit("mmap");
            
            if (sem_wait(sem_full) == -1)
                    errExit("sem_wait");
            if (sem_wait(sem_mutex) == -1)
                errExit("sem_wait");

            place2->index1 = place2->index1 - 1;
            place2->index2 = place2->index2 - 1;
            
            if (sem_post(sem_mutex) == -1)
                errExit("sem_post");
            if (sem_post(sem_empty) == -1)
                errExit("sem_post");

            
            printf("Consumer %d (PID=%ld) exiting...\n", j, (long)getpid());
            _exit(EXIT_SUCCESS);

        default: /* Parent - loops to create next child */
            break;
        }
    }
    sigemptyset(&emptyMask);
    while (numLiveChildren > 0)
    {
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend");
        sigCnt++;
    }
    printf("index1Vac::%d\n",place->index1);
    printf("index2Vac::%d\n",place->index2);
    printf("\nAll children have terminated; SIGCHLD was caught "
           "%d times\n",
           sigCnt);

    exit(EXIT_SUCCESS);
}