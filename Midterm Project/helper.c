#include "helper.h"
volatile __sig_atomic_t exitSignal = 0;
void checkArguments(int argc, char **argv, args *givenArgs)
{

    int opt,res;
    //initialize all values to zero
    givenArgs->bArg = 0;
    givenArgs->cArg = 0;
    givenArgs->nArg = 0;
    givenArgs->tArg = 0;
    givenArgs->vArg = 0;
    if (argc < 13)
    {
        showUsageAndExit();
    }
    //beforehand get t and c values
    res = atoi(argv[10]);
    if (res < 1)
    {
        showUsageAndExit();
    }
    else{
         givenArgs->tArg = res;
    }
    res = atoi(argv[6]);
    if (res < 3)
    {
        showUsageAndExit();
    }
    else{
         givenArgs->cArg = res;
    }
    while ((opt = getopt(argc, argv, "n:v:c:b:t:i:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            res = atoi(optarg);
            if (res < 2)
            {
                showUsageAndExit();
            }
            givenArgs->nArg = res;
            break;
        case 'v':
            res = atoi(optarg);
            if (res < 2)
            {
                showUsageAndExit();
            }
            givenArgs->vArg = res;
            break;
        case 'c':
            res = atoi(optarg);
            if (res < 3)
            {
                showUsageAndExit();
            }
            givenArgs->cArg = res;
            break;
         case 'b':
            res = atoi(optarg);
            int a = givenArgs->tArg * givenArgs->cArg + 1;
            if (res < a)
            {
                showUsageAndExit();
            }
            givenArgs->bArg = res;
            break;
        case 't':
            res = atoi(optarg);
            if (res < 1)
            {
                showUsageAndExit();
            }
            givenArgs->tArg = res;
            break;
       
        case 'i':
            strcpy(givenArgs->iArg , optarg);
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
    printf("Usage: ./program [FLAGS] and [PARAMETERS]\n"
           "Flags and Parameters:\n"
           "-n >= 2: the number of nurses (integer)\n"
           "-v >= 2: the number of vaccinators (integer)\n"
           "-c >= 3: the number of citizens (integer)\n"
           "-b >= tc+1: size of the buffer (integer)\n"
           "-t >= 1: how many times each citizen must receive the 2 shots (integer)\n"
           "-i: pathname of the input file\n"
           "Example\n"
           "./program –n 3 –v 2 –c 3 –b 11 –t 3 –i /home/user/Desktop/test\n");
        exit(EXIT_FAILURE);
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
char readOneChar(int fd){
    char c;
    safeRead(fd, &c, 1);
    if (c != '\n')
    {
        return c;
    }
    safeRead(fd, &c, 1);
    return c;
}
/*int main(int argc, char *argv[])
{
    //start search operation
    args givenParams;
    checkArguments(argc, argv, &givenParams);
    printf("n:%d\n",givenParams.nArg);
    printf("v:%d\n",givenParams.vArg);
    printf("c:%d\n",givenParams.cArg);
    printf("b:%d\n",givenParams.bArg);
    printf("t:%d\n",givenParams.tArg);
    printf("i:%s\n",givenParams.iArg);
    return 0;
}*/
