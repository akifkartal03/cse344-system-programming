#include "helper.h"

void checkArguments(int argc, char **argv, args *givenArgs)
{

    if (argc != 4)
    {
        showUsageAndExit();
    }
    strcpy(givenArgs->hwFile, argv[1]);
    strcpy(givenArgs->studentFile, argv[2]);
    givenArgs->money = atof(argv[3]);
}

void showUsageAndExit()
{
    printf("Usage: ./program [PARAMETERS]\n"
           "Parameters:\n"
           "homeworkFilePath: pathname of the input file 1\n"
           "studentsFilePath: pathname of the input file 2\n"
           "money: total money the cheater student has.\n"
           "Example:\n"
           "./program /home/user/Desktop/test1 /home/user/Desktop/test2 10000\n");
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
void initStudents(student students[], int fd, pthread_t tids[])
{
    char *line;
    int n = getNumberOfLine(fd);
    for (int i = 0; i < n; i++)
    {
        line = readLine(fd, i + 1);
        seperateLine(line,students[i]);
        students[i].id = tids[i];
        students->index = i;
        students->isBusy = 0;
        students->isNotified = 0;
    }
    free(line);
}
void seperateLine(char *line, student *std)
{
    char *p = strtok(line, " \n");
    int i = 0;
    while (p != NULL)
    {
        switch (i)
        {
        case 0:
            strcpy(std->name,p);
            break;
        case 1:
            std->quality = atof(p);
            break;
        case 2:
            std->speed = atof(p);
            break;
        case 3:
            std->price = atof(p);
            break;
        default:
            break;
        }
        p = strtok(NULL, " \n");
        i++;
    }
}

/*int main(int argc, char *argv[])
{

    args givenParams;
    checkArguments(argc, argv, &givenParams);
    printf("hw:%s\n", givenParams.hwFile);
    printf("std:%s\n", givenParams.studentFile);
    printf("money:%.2f\n", givenParams.money);

    student std;
    char temp[] = "this is a test sentence.\n";
    seperateLine(temp, &std);

    return 0;
}*/