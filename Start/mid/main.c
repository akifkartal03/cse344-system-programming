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
typedef struct arg
{
    
   int nArg;
   int vArg;
   int cArg;
   int bArg;
   int tArg;
   char iArg[41];

}args;

static char memoryName[50];
sem_t *sem_full;
sem_t *sem_empty;
sem_t *sem_mutex;
sem_t *sem_index;
void errExit(char *msg){
    //In case of an arbitrary error, 
    //exit by printing to stderr a nicely formatted informative message.
    fprintf(stderr, "%s:%s\n",msg,strerror(errno));
    exit(EXIT_FAILURE);
}
void sigchldHandler(int sig)
{
    if (sig == SIGCHLD)
    {
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
}

void showUsageAndExit()
{
    printf("Usage: ./program [FLAGS] and [PARAMETERS]\n"
           "Flags and Parameters:\n"
           "-n >= 2: the number of nurses (integer)\n"
           "-v >= 2: the number of vaccinators (integer)\n"
           "-c >= 3: the number of citizens (integer)\n"
           "-b >= tc+1: size of the buffer (integer)\n"
           "-t >= 1: how many times each citizen must receive the 2 shots (integer)\n"
           "-i: pathname of the input file\n"
           "Example\n"
           "./program –n 3 –v 2 –c 3 –b 11 –t 3 –i /home/user/Desktop/test\n");
        exit(EXIT_FAILURE);
}
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
    sem_unlink("full");
    sem_unlink("empty4");
    sem_unlink("mutex");
    sem_unlink("index");    
    shm_unlink(memoryName);
}
void checkArguments(int argc, char **argv, args *givenArgs)
{

    int opt,res;
    //initialize all values to zero
    givenArgs->bArg = 0;
    givenArgs->cArg = 0;
    givenArgs->nArg = 0;
    givenArgs->tArg = 0;
    givenArgs->vArg = 0;
    if (argc < 13)
    {
        showUsageAndExit();
    }
    //beforehand get t and c values
    res = atoi(argv[10]);
    if (res < 1)
    {
        showUsageAndExit();
    }
    else{
         givenArgs->tArg = res;
    }
    res = atoi(argv[6]);
    if (res < 3)
    {
        showUsageAndExit();
    }
    else{
         givenArgs->cArg = res;
    }
    while ((opt = getopt(argc, argv, "n:v:c:b:t:i:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            res = atoi(optarg);
            if (res < 2)
            {
                showUsageAndExit();
            }
            givenArgs->nArg = res;
            break;
        case 'v':
            res = atoi(optarg);
            if (res < 2)
            {
                showUsageAndExit();
            }
            givenArgs->vArg = res;
            break;
        case 'c':
            res = atoi(optarg);
            if (res < 3)
            {
                showUsageAndExit();
            }
            givenArgs->cArg = res;
            break;
         case 'b':
            res = atoi(optarg);
            int a = givenArgs->tArg * givenArgs->cArg + 1;
            if (res < a)
            {
                showUsageAndExit();
            }
            givenArgs->bArg = res;
            break;
        case 't':
            res = atoi(optarg);
            if (res < 1)
            {
                showUsageAndExit();
            }
            givenArgs->tArg = res;
            break;
       
        case 'i':
            strcpy(givenArgs->iArg , optarg);
            break;
        case '?':
            showUsageAndExit();
            break;
        default:
            showUsageAndExit();
            break;
        }
        
    }
     
    
}

void remove_vaccine(char *buffer, int size, int index)
{
   for(int i = index; i < size - 1; i++){
       buffer[i] = buffer[i + 1];
   } 
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

int main(int argc, char *argv[])
{
    args givenParams;
    checkArguments(argc, argv, &givenParams);
    int fd = safeOpen(givenParams.iArg, O_RDWR);
    int memFd;
    pid_t pid;
    struct stat sb;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    sem_full = sem_open("full", O_CREAT, 0666, 0);
    if (sem_full == SEM_FAILED)
        errExit("sem_open error!");
    int n = givenParams.bArg;
    printf("n:%d\n",n);
    sem_empty = sem_open("empty4", O_CREAT, 0666, n);
    if (sem_empty == SEM_FAILED)
        errExit("sem_open error!");
    sem_mutex = sem_open("mutex", O_CREAT, 0666, 1);
    if (sem_mutex == SEM_FAILED)
        errExit("sem_open error!");
    sem_index = sem_open("index", O_CREAT, 0666, 0);
    if (sem_index == SEM_FAILED)
        errExit("sem_open error!");
    strcpy(memoryName,"clinic");
    memFd = shm_open(memoryName, O_CREAT | O_RDWR, mode);
    if (memFd == -1)
        errExit("shm_open error!");
    if (fstat(memFd, &sb) == -1)
        errExit("fstat error");
    if (ftruncate(memFd, n) == -1)  
        errExit("ftruncate error");
    
    /*attach itself to the shared memory segment*/
    char *buffer = (char *)mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
    if (buffer == MAP_FAILED)
        errExit("mmap");
    /*for (int i = 0; i < n; ++i) {
        buffer[i] = readOneChar(fd);
    }
    for (int i = 0; i < n; ++i) {
        printf("%c",buffer[i]);
    }
    printf("\n");*/
    /*save remove method*/
    if (atexit(removeAll) != 0)
        errExit("atexit");

    int sigCnt;
    sigset_t blockMask, emptyMask;
    struct sigaction sa;

    sigCnt = 0;
    numLiveChildren = givenParams.nArg + givenParams.vArg;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigchldHandler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction");

    /* Block SIGCHLD to prevent its delivery if a child terminates
       before the parent commences the sigsuspend() loop below */

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCHLD);
    if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1)
        errExit("sigprocmask");


    

    for (int i = 0; i < numLiveChildren; i++)
    {
        pid = fork();
        if (pid == -1)
        {
            errExit("fork error!");
        }
        else if (pid == 0)
        {
            /* child */
            char *nurse_buffer = (char *)mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
             if (nurse_buffer == MAP_FAILED)
                errExit("mmap");
            if (i < givenParams.nArg)
            {
                //producer
                printf("Nurse Child (PID=%ld) running...\n",(long) getpid());
                    /*create kabinet*/
                while(1){
        
                    if (sem_wait(sem_empty) == -1)
                        errExit("sem_wait");
                    if (sem_wait(sem_mutex) == -1)
                        errExit("sem_wait");
                    /*add to buffer*/
                    char vaccine = readOneChar(fd);  
                    if (vaccine != 'x')
                    {
                        int counter;
                        if (sem_getvalue(sem_index, &counter) == -1)
                            errExit("sem_get");
                        nurse_buffer[counter] = vaccine;
                        if (sem_post(sem_index) == -1)
                            errExit("sem_post");
                        printf("counter:%d, ",counter);
                        printf("vac:%c\n",vaccine);
                    }
                    else
                    {
                        if (sem_post(sem_mutex) == -1)
                            errExit("sem_post");
                        break;    
                        
                    }
                    if (sem_post(sem_mutex) == -1)
                        errExit("sem_post");
                    if (sem_post(sem_full) == -1)
                        errExit("sem_post");  
                }
                printf("Nurse Child (PID=%ld) exiting...\n",(long) getpid());    
                _exit(EXIT_SUCCESS);
            }
            else{
                //customer
                printf("vaccinator Child (PID=%ld) running...\n",(long) getpid());
                while(1){
                   
                    if (sem_wait(sem_full) == -1)
                        errExit("sem_wait");
                    if (sem_wait(sem_mutex) == -1)
                        errExit("sem_wait");
                    /*remove from buffer*/
                    int isFound1 = 0;
                    int isFound2 = 0;
                    for (int i = 0; i < n; i++)
                    {
                        if (nurse_buffer[i] == '1')
                        {
                            remove_vaccine(nurse_buffer,n,i);
                            isFound1 = 1;
                            if (isFound2)
                            {
                                if (sem_wait(sem_index) == -1)
                                    errExit("sem_wait");
                                if (sem_wait(sem_index) == -1)
                                    errExit("sem_wait");
                                break;
                            }
                            
                        }
                        if (nurse_buffer[i] == '2')
                        {
                            remove_vaccine(nurse_buffer,n,i);
                            isFound2 = 1;
                            if (isFound1)
                            {
                                if (sem_wait(sem_index) == -1)
                                    errExit("sem_wait");
                                if (sem_wait(sem_index) == -1)
                                    errExit("sem_wait");
                                break;
                            }
                        }
                    }
                    
                    if (sem_post(sem_mutex) == -1)
                        errExit("sem_post");
                    if (sem_post(sem_empty) == -1)
                        errExit("sem_post");  
                }
                printf("vaccinator Child (PID=%ld) exiting...\n",(long) getpid());    
                _exit(EXIT_SUCCESS);
            }
            
        }
        else
        {
            /* parent */


        }
        
        
        
    }
    
    sigemptyset(&emptyMask);
    while (numLiveChildren > 0) {
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend");
        sigCnt++;
    }
    printf("buffer:\n");
    for (int i = 0; i < n; i++)
    {
        printf("%c",buffer[i]);
    }
    printf("\nAll children have terminated; SIGCHLD was caught "
            "%d times\n", sigCnt);

    exit(EXIT_SUCCESS);
}