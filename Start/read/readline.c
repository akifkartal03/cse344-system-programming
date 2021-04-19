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
    srand(time(0));
    return (rand() % n) + 1;
}
int main(int argc, char **argv)
{
    
    //int fd = safeOpen(argv[1], O_RDWR);
    //printf("%s\n",readLine(fd,1));
    printf("%d\n", getRandom(3));
    return 0;
}