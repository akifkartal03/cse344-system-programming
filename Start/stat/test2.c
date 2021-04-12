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

#define row 8
#define column 8

void myStderr(const char *str)
{
    ssize_t size = strlen(str);
    if (size != write(STDERR_FILENO, str, size))
    {
        perror("write system call error! ");
        exit(EXIT_FAILURE);
    }
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
void readFile(int fd,double x[row][column],double y[row][column]){
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
        if (c == '\n')
        {
            buffer[i] = ',';
        }
        else{
            buffer[i] = c;
        }
        i++;
        
    } while (bytes_read == 1);
    char *pt = strtok(buffer, ",");
    int counter = 3; //to keep track of two coordinate
    i = 1;
    int k=0,m=0,k1 = 0,m1 =0;
    double temp;
    while (pt != NULL)
    {
        double next = atof(pt);
        
        if (i % 2 == 1)
        {
            x[k][m] =next;
            m++;
        }
        else{
            y[k][m1] =next;
            m1++;
        }
        if (i%16 == 0)
        {
            
            k++;
            m=0;
            m1=0;
            
        }
        i++;
        pt = strtok(NULL, ",");
    }
    safeLseek(fd, 1, SEEK_SET);
    safeLseek(fd, -1, SEEK_CUR);

}
/*void extendFile(int fd,int extendSize,int offset,char *buffer){
    struct stat fileStat;
    fstat(fd, &fileStat);
    int fileSize = (int)fileStat.st_size;
    ftruncate(fd, fileSize+extendSize);
    char space = (char)32;
    safeLseek(fd, offset, SEEK_SET);

    

}*/
int main(int argc, char **argv)
{
    if (argc !=2)
        return 1;
    int fd = safeOpen(argv[1], O_RDWR);
    double x[row][column] ,y[row][column];
    readFile(fd,x,y);
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%.1f,%.1f,",x[i][j],y[i][j]);
        }
        printf("\n");
        
    }
    
    /*char c1;
    double number = 15.0;
    int line = atoi(argv[2]);
    char *buf = (char *)calloc(100, sizeof(char)); 
    int capacity = 0;
    int i = 0; 
    char c;
    int offset = 0;
    int bytes_read;
    do
    {

        bytes_read = safeRead(fd, &c, 1);
        offset += bytes_read;
        if (capacity <= offset + 1)
        {
            capacity = capacity + 100;
            buf = realloc(buf, capacity * sizeof(char));
        }
        buf[i] = c;
        i++;

    } while (bytes_read == 1);

    safeLseek(fd, 1, SEEK_SET);
    for (int i = 0; i < line; i++)
    {
        safeRead(fd, &c1, 1);
        while (c1 != '\n')
        {
            safeRead(fd, &c1, 1);
        }
    }
   
    int loc = safeLseek(fd, -1, SEEK_CUR);
    char buffer[30];
    int n = sprintf(buffer, ",%.1f\n", number);
    struct stat fileStat;
    fstat(fd, &fileStat);
    int fileSize = (int)fileStat.st_size;
    ftruncate(fd, fileSize+n-1);
    safeLseek(fd, loc, SEEK_SET);
    int a = safeWrite(fd, buffer, n);
    safeWrite(fd, &buf[loc+1], fileSize-loc-1);
    free(buf);*/
    close(fd);
    return 0;

}