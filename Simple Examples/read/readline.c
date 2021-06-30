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

    } while (c != '\n' && bytes_read == 1);
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
    if(c == '\n')
        return i-1;
    else
        return i+1;
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
    int fdHw = safeOpen("/home/akif/Desktop/source/example", O_RDONLY);
    printf("%d\n", getNumberOfLine(fdHw));
    printf("%s\n", readLine(fdHw,5));
}