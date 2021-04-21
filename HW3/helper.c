#include "helper.h"
volatile __sig_atomic_t exitSignal = 0;
void checkArguments(int argc, char **argv, struct args *givenArgs)
{

    int opt,res;
    //initialize all flags to zero
    givenArgs->count = 0;
    givenArgs->sFlag = 0;
    givenArgs->fFlag = 0;
    givenArgs->bFlag = 0;
    givenArgs->mFlag = 0;
    if (argc < 9)
    {
        showUsageAndExit();
    }
    while ((opt = getopt(argc, argv, "b:s:f:m:")) != -1)
    {
        switch (opt)
        {
        case 'b':
            res = atoi(optarg);
            if (res < 0)
            {
                showUsageAndExit();
            }
            givenArgs->bFlag = 1;
            strcpy(givenArgs->bArg , optarg);
            break;
        case 's':
            givenArgs->sFlag = 1;
            strcpy(givenArgs->sArg , optarg);
            break;
        case 'f':
            givenArgs->fFlag = 1;
            strcpy(givenArgs->fArg , optarg);
            break;
        case 'm':
            givenArgs->mFlag = 1;
            strcpy(givenArgs->mArg , optarg);
            break;
        case '?':
            showUsageAndExit();
            break;
        default:
            showUsageAndExit();
            break;
        }
        if (exitSignal)
        {
            printf("You are exiting...\n");
            exit(EXIT_SUCCESS);
        }
    }
    
}

void showUsageAndExit()
{
    printf("Usage: ./player [FLAGS] and [PARAMETERS]\n"
           "Flags and Parameters:\n"
           "-b : haspotatoornot, can be 0 or a positive integer. \n"
           "-s : nameofsharedmemory, this will be the name of the posix named shared memory\n"
           "-f: filewithfifonames ,this ascii file will contain the names of the fifoes\n"
           "-m: namedsemaphore, named posix semaphore to be used for synchronization\n"
           "Example\n"
           "./player –b 5 –s shr_mem –f fifo_names –m sem\n");
        exit(EXIT_FAILURE);
}
int getRandom(int n)
{
    return (rand() % n) + 1;
}
void exitHandler(int signal)
{
    if (signal == SIGINT)
    {
        exitSignal = 1;
    }
}
void errExit(char *msg){
    //In case of an arbitrary error, 
    //exit by printing to stderr a nicely formatted informative message.
    fprintf(stderr, "%s:%s\n",msg,strerror(errno));
    exit(EXIT_FAILURE);
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
char *readLine(int fd,int line){
    char c1;
    safeLseek(fd, 0, SEEK_SET);
    for (int i = 0; i < line-1; i++)
    {
        do{
           safeRead(fd, &c1, 1);
        }while (c1 != '\n');
    }
    int offset = 0;
    int bytes_read;
    int capacity = 0;
    int i = 0;
    char c;
    char *buffer = (char *)calloc(50, sizeof(char));    
    do{
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
    buffer[i-1] = '\0';
    return buffer;
}
int getNumberOfLine(int fd){
    int bytes_read;
    int i = 0;
    char c;
    safeLseek(fd, 0, SEEK_SET);   
    do{
        bytes_read = safeRead(fd, &c, 1); 
        if (c == '\n')
        {
            i++;
        }
    } while (bytes_read == 1);
    return i - 1;
}