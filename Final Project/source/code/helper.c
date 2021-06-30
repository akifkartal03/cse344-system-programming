#include "helper.h"
int LogFD = 0;
void checkArguments(int argc, char **argv, args *givenArgs)
{
    if (argc != 9)
    {
        showUsageAndExit();
    }
    int opt,res;
    while ((opt = getopt(argc, argv, "p:o:l:d:")) != -1)
    {
        switch (opt)
        {
            case 'p':
                res = atoi(optarg);
                if (res < 0 || res > 65535)
                {
                    fprintf(stderr, "Port should be in range [0, 65535]!");
                    showUsageAndExit();
                }
                givenArgs->port = res;
                break;
            case 'o':
                givenArgs->logFd = safeOpen(optarg, O_CREAT | O_WRONLY | O_EXCL,0);
                LogFD = givenArgs->logFd;
                break;
            case 'l':
                res = atoi(optarg);
                if (res < 2)
                {
                    showUsageAndExit();
                }
                givenArgs->poolSize = res;
                break;
            case 'd':
                givenArgs->datasetFd = safeOpen(optarg, O_RDONLY,0);
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

void showUsageAndExit()
{
    printf("Usage: ./server [FLAGS] and [PARAMETERS]\n"
           "Flags and Parameters:\n"
           "-p: Port, this is the port number the server will use for incoming connections. \n"
           "-o: pathToLogFile, this is the relative or absolute path of the log file.\n"
           "-l: poolSize, the number of threads in the pool (>= 2)\n"
           "-d: datasetPath, is the relative or absolute path of a csv file containing a single table,\n"
           "where the first row contains the column names.\n"
           "Example\n"
           "./server -p 34567 -o /home/akif/sysprog/logfile –l 8 –d /home/akif/sysprog/dataset.csv\n");
    exit(EXIT_FAILURE);
}

void errExit(char *msg,int toLog)
{
    //In case of an arbitrary error,
    //exit by printing to stderr a nicely formatted informative message.
    if(toLog)
        dprintf(LogFD, "[%s]%s:%s\n", getTime(),msg, strerror(errno));
    else
        fprintf(stderr, "%s:%s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}
int safeRead(int fd, void *buf, size_t size,int toLog)
{
    int rd = read(fd, buf, size);
    if (rd == -1)
    {
        errExit("reading error!",toLog);
    }
    return rd;
}
int safeWrite(int fd, void *buf, size_t size,int toLog)
{
    int wrt = write(fd, buf, size);
    if (wrt == -1)
    {
        errExit("writing error!",toLog);
    }
    return wrt;
}
int safeOpen(const char *file, int oflag,int toLog)
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    int fd = open(file, oflag, mode);
    if (fd < 0)
    {
        if (errno == EEXIST)
        {
            remove(file);
            return safeOpen(file, oflag,toLog);
        }
        errExit("open error!",toLog);
    }
    return fd;
}
int safeLseek(int fd, int offset, int whence,int toLog)
{
    int pos = lseek(fd, offset, whence);
    if (pos == -1)
    {
        errExit("lseek error!",toLog);
    }
    return pos;
}

char *getTime(){
    time_t localTime;
    localTime=time(NULL);
    char *str = asctime( localtime(&localTime));
    char *removed = strchr(str, '\n');
    removed[0] = '\0';
    return str;
}
void milSleep(int milSecond){
    int msRemaining = milSecond % 1000;
    long uSec = msRemaining * 1000;
    struct timespec tsSleep;

    tsSleep.tv_sec = milSecond / 1000;
    tsSleep.tv_nsec = 1000*uSec;
    nanosleep(&tsSleep, NULL);
}
