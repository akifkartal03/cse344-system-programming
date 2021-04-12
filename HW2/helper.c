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
int safePRead(int fd, void *buf, size_t size,int ofset)
{
    int rd = pread(fd, buf, size,ofset);
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
int safePWrite(int fd, void *buf, size_t size,int offset)
{
    int wrt = pwrite(fd, buf, size,offset);
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
    safeWrite(fd,buffer,n);
    safeWrite(fd, &buf[position+1], fileSize-position-1);
    //safePWrite(fd,buffer,n,position);
    //safePWrite(fd, &buf[position+1], fileSize-position-1,position+n);
    safeLseek(fd, 1, SEEK_SET);
    safeLseek(fd, -1, SEEK_CUR);
}
void readFile(int fd,double x[][],double y[][],int add){
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
        if (i%(16+add) == 0)
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
    free(buffer);

}
char* readFile2(int fd){
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
    safeLseek(fd, 1, SEEK_SET);
    safeLseek(fd, -1, SEEK_CUR);
    return buffer;
}
void readLine(double x[row][column],double y[row][column],double x1[],double y1[],int lineNumber){
    
    for (int i = 0; i < 8; i++)
    {
        x1[i] =x[lineNumber][i];
        y1[i] =y[lineNumber][i];
    }
    

}
double round1_error(int fd){
    double sum=0.0,avg=0.0;
    double x1[row][column+2] ,y1[row][column+2];
    readFile(fd,x1,y1,1);
    for (int k = 1; k < 9; k++)
    {
        double fx6 = y1[k-1][7];
        double px6 = x1[k-1][8];
        sum+=estimationError(fx6,px6);
    }
    avg = sum/8.0;
    safeLseek(fd, 1, SEEK_SET);
    safeLseek(fd, -1, SEEK_CUR);
    return avg; 
    
}
double round2_error(int fd){
    double sum=0.0,avg=0.0;
    double x1[row][column+2] ,y1[row][column+2];
    readFile(fd,x1,y1,2);
    for (int k = 1; k < 9; k++)
    {
        double fx6 = y1[k-1][7];
        double px6 = y1[k-1][8];
        sum+=estimationError(fx6,px6);
    }
    avg = sum/8.0;
    safeLseek(fd, 1, SEEK_SET);
    safeLseek(fd, -1, SEEK_CUR);
    return avg; 
    
}