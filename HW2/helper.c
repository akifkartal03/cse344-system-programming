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
void lockFile(int fd, struct flock *fl, int read);
void unlockFile(int fd, struct flock *fl);
void writeEndofLine(int fd,double number,int line){
    char c;
    safeLseek(fd,1,SEEK_SET);
    for (int i = 0; i < line; i++)
    {
        safeRead(fd,&c,1);
        while (c !='\n')
        {
            safeLseek(fd,1,SEEK_CUR);
            safeRead(fd,&c,1);
        }
    }
    safeLseek(fd,-1,SEEK_CUR);
    char buffer[30];
    int n = sprintf(buffer, ",%.1f,%.1f\n",number,number);
    printf("n:%d\n%s\n",n,buffer);
    int a = safeWrite(fd,buffer,n);
    printf("a:%d\n",a);
}

double calculateInterpolation(double x[], double y[], int xCount,int count)
{
    double yResult = 0;
    for (int i = 0; i < count; i++)
    {
        double nextY = y[i];
        for (int j = 0; j < count; j++)
        {
            if (j != i)
                nextY = nextY * (xCount - x[j]) / (x[i] - x[j]);
        }
        yResult = yResult + nextY;
    }
    return yResult;
}
void testLagrange(char *buff,int count){
    char *pt;
    pt = strtok (buff,",");
    int counter = 3;
    double x[count],y[count],xi,yi;
    int i = 0;
    double temp;
    const int cplus1 = count + 1;
    const int citself = count;
    while (pt != NULL) {
        double a = atof(pt);
        if (i>count+1)
        {
            break;
        }
        if (i == count)
        {
            xi=a;
            i++;
        }
        if (i == count + 1)
        {
            yi=a;
            i++;
        }
        if (counter % 2 == 0 && i < citself)
        {
            x[i] = temp;
            y[i] = a;
            i++;
        }
        pt = strtok (NULL, ",");
        temp = a;
        counter++;
    }
    printf("res: %.1f\n",calculateInterpolation(x,y,xi,4));
}
int main(int argc, char *argv[])
{
    int offset = 0;
    int bytes_read;
    int capacity = 0;
    checkArgument(argc);
    char *path = argv[1];
    int i = 0;
    char c;
    int fd = safeOpen(path, O_RDONLY);
    char *buffer = (char *) calloc(100, sizeof(char));
    /* Read from the file, one chunk at a time. Continue until read “comes up short”, that is, 
    reads less than we asked for. This indicates that we’ve hit the end of the file.
    do
    {
        
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
    /*i = 0;
    while (read(fd, &c, 1) == 1)
    {
        buffer[i] = c;
        i++;
        /* end of line
        if (c == '\n')
        {
            buffer[i] = 0;
            if (!strncmp(buffer, w, strlen(w)))
            {
                printf("w was found in line %d\n", line);
                puts(buffer);
                n++;
            }
            line++;
            i = 0;
            continue;
        }
        i++; *
    }*/
    /*char *pt;
    pt = strtok (buffer,",");
    while (pt != NULL) {
        double a = atof(pt);
        printf("%.1f\n", a);
        pt = strtok (NULL, ",");
    }
    testLagrange(buffer,4);
    /* All done.*/
    //close(fd);
    //int fd2=safeOpen(path, O_RDWR | O_APPEND);
    //writeEndofLine(fd2,8.0,1);

    //free(buffer);

    


    return 0;
}
