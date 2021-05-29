#include "helper.h"
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
                    exit(EXIT_FAILURE);
                }
                givenArgs->port = res;
                break;
            case 'o':
                givenArgs->logFd = safeOpen(optarg, O_CREAT | O_WRONLY);
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
                givenArgs->datasetFd = safeOpen(optarg, O_RDONLY);
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
           "-p : Port, this is the port number the server will use for incoming connections. \n"
           "-o : pathToLogFile, this is the relative or absolute path of the log file.\n"
           "-l: poolSize, the number of threads in the pool (>= 2)\n"
           "-d: datasetPath, is the relative or absolute path of a csv file containing a single table, where the first\n"
           "row contains the column names.\n"
           "Example\n"
           "./server -p 34567 -o /home/erhan/sysprog/logfile –l 8 –d /home/erhan/sysprog/dataset.csv\n");
    exit(EXIT_FAILURE);
}

void errExit(char *msg)
{
    //In case of an arbitrary error,
    //exit by printing to stderr a nicely formatted informative message.
    fprintf(stderr, "%s:%s\n", msg, strerror(errno));
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
char readOneChar(int fd)
{
    char c;
    int eof;
    // x represent end of file
    eof = safeRead(fd, &c, 1);
    if (eof != 0)
    {
        while (c == '\n')
        {
            eof = safeRead(fd, &c, 1);
            if (eof == 0)
            {
                return 'x';
            }
        }
        return c;
    }
    return 'x';
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

/*int main(int argc, char *argv[])
{

    args givenParams;
    checkArguments(argc, argv, &givenParams);
    printf("port:%d\n", givenParams.port);
    printf("logfd:%d\n", givenParams.logFd);
    printf("poolSize:%d\n", givenParams.poolSize);
    printf("dataset:%d\n", givenParams.datasetFd);
    char buffer[25] = "this last\na test file.";
    write(givenParams.logFd,buffer,20);
    return 0;
}*/