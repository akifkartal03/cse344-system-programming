#include "helper.h"


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
void lockFile(int fd){
    struct flock lock;
    memset(&lock, '\0', sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        myStderr("lock file error!\n");
        exit(EXIT_FAILURE);
    }
}
void readLockFile(int fd){
    struct flock lock;
    memset(&lock, '\0', sizeof(lock));
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        myStderr("lock file error!\n");
        exit(EXIT_FAILURE);
    }
}
void unlockFile(int fd){
    struct flock lock;
    memset(&lock, '\0', sizeof(lock));
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        myStderr("unlock file error!\n");
        exit(EXIT_FAILURE);
    }
}
void writeEndofLine(int fd, double number, int line,char *buf)
{
    char c1;
    safeLseek(fd, 1, SEEK_SET);
    for (int i = 0; i < line; i++)
    {
        safeRead(fd, &c1, 1);
        while (c1 != '\n')
        {
            safeRead(fd, &c1, 1);
        }
    }
    int position = safeLseek(fd, -1, SEEK_CUR);
    char buffer[30];
    int n = sprintf(buffer, ",%.1f\n", number);
    struct stat fileStat;
    fstat(fd, &fileStat);
    int fileSize = (int)fileStat.st_size;
    ftruncate(fd, fileSize+n-1);
    pwrite(fd,buffer,n,position);
    pwrite(fd, &buf[position+1], fileSize-position-1,position+n);
}


char* readFile(int fd){
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
        buffer[i] = c;
        i++;
        
    } while (bytes_read == 1);
    return buffer;
}
void readLine(int fd,char *buff,int lineNumber,int numberOfCoor,double x[],double y[],double *xi){
    char c1;
    safeLseek(fd, 1, SEEK_SET);
    for (int i = 0; i < lineNumber-1; i++)
    {
        safeRead(fd, &c1, 1);
        while (c1 != '\n')
        {
            safeRead(fd, &c1, 1);
        }
    }
    int position;
    if (lineNumber == 1)
        position = safeLseek(fd, -1, SEEK_CUR);
    else
        position = safeLseek(fd, 0, SEEK_CUR);
    char *pt = strtok(&buff[position], ",");
    int counter = 3; //to keep track of two coordinate
    int i = 0;
    double temp;
    while (pt != NULL)
    {
        double a = atof(pt);
        if (i > numberOfCoor + 1)
        {
            break;
        }
        if (i == numberOfCoor)
        {
            *xi = a;
            i++;
        }
        if (i == numberOfCoor + 1)
        {
            i++;
        }
        if (counter % 2 == 0 && i < numberOfCoor)
        {
            x[i] = temp;
            y[i] = a;
            i++;
        }
        pt = strtok(NULL, ",");
        temp = a;
        counter++;
    }
    //free(pt);
    

}
double round1_error(int fd){
    double sum=0.0,avg=0.0;
    char *b=readFile(fd);
    for (int k = 1; k < 9; k++)
    {
        double fx6,px6;
        px6= getRound1Result(fd,b,k,17,&fx6,13);
        sum+=estimationError(fx6,px6);
    }
    avg = sum/8.0;
    free(b);
    return avg; 
}
double round2_error(int fd){
    double sum=0.0,avg=0.0;
    char *b=readFile(fd);
    for (int k = 1; k < 9; k++)
    {
        double fx7,px7;
        px7= getRound1Result(fd,b,k,18,&fx7,15);
        sum+=estimationError(fx7,px7);
    }
    avg = sum/8.0;
    free(b);
    return avg; 
    
}
double getRound1Result(int fd,char *buff,int lineNumber,int numberOfCoor,double *yi,int count){
    char c1;
    safeLseek(fd, 1, SEEK_SET);
    for (int i = 0; i < lineNumber-1; i++)
    {
        safeRead(fd, &c1, 1);
        while (c1 != '\n')
        {
            safeRead(fd, &c1, 1);
        }
    }
    int position;
    if (lineNumber == 1)
        position = safeLseek(fd, -1, SEEK_CUR);
    else
        position = safeLseek(fd, 0, SEEK_CUR);
    char *pt = strtok(&buff[position], ",");
    
    int i = 0;
    double temp;
    while (pt != NULL && i<numberOfCoor)
    {
        double a = atof(pt);
        pt = strtok(NULL, ",");
        temp = a;
        if (i == count)
        {
            *yi =a;
        }
        i++;
    }
    safeLseek(fd, 1, SEEK_SET);
    safeLseek(fd, -1, SEEK_CUR);
    //free(pt);
    return temp;
}