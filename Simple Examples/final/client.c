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
#include <pthread.h>
#include <float.h>
#include <limits.h>

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
int safeWrite(int fd, void *buf, size_t size)
{
    int wrt = write(fd, buf, size);
    if (wrt == -1)
    {
        errExit("writing error!\n");
    }
    return wrt;
}
int safeOpen(const char *file, int oflag)
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    int fd = open(file, oflag, mode);
    if (fd < 0)
    {
        if (errno == EEXIST)
        {
            remove(file);
            return safeOpen(file, oflag);
        }
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
int isMyLine(char *line , int id){
    char temp[strlen(line) + 1];
    strcpy(temp,line);
    char *num = strtok(temp," ");
    if(num != NULL) {
        if (atoi(num) == id) {
            return 1;
        }
    }
    return 0;
}

char *getLine(int fd,int id){

    int offset = 0;
    int bytes_read;
    int capacity = 50;
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

        if (c == '\n')
        {
            if(bytes_read != 0){
                if(isMyLine(buffer,id)){
                    buffer[i] = '\0';
                    return buffer;
                }
                else{
                    free(buffer);
                    buffer = (char *)calloc(50, sizeof(char));
                    capacity = 50;
                    offset = 0;
                    i = 0;
                }
            }
        }
        else{
            if(bytes_read != 0){
                buffer[i] = c;
                i++;
            }
            else{
                if(isMyLine(buffer,id)){
                    buffer[i] = '\0';
                    return buffer;
                }
            }
        }
    } while (bytes_read == 1);
    return NULL;
}

int main(int argc, char *argv[]){
    int id = atoi(argv[1]);
    int fd = safeOpen("example",O_RDONLY);
    safeLseek(fd, 0, SEEK_SET);
    char *line = getLine(fd,id);
    printf("\n\n\n\nid:%d\n",id);
    while(line != NULL){
        printf("%s\n",line);
        line = getLine(fd,id);
    }
    return 0;
}

