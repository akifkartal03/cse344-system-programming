#include "helper.h"

int main(int argc, char *argv[])
{
    /*CTRL-C signal handling with sigaction*/
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &exitHandler;
    sigaction(SIGINT, &sa, NULL);

    args givenParams;
    checkArguments(argc, argv, &givenParams);
    int myFd, dummyFd, reciverFd, fd, fifoNames;
    //char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct sender req;
    struct reciever resp;
    struct stat sb;
    player *data;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    fifoNames = safeOpen(givenParams.fArg, O_RDWR);
    fd = shm_open(givenParams.sArg, O_RDWR, mode);
    if (fd == -1)
        errExit("shm_open error!");
    if (fstat(fd, &sb) == -1)
        errExit("fstat error");
    size_t len = 4096 if (sb.st_size == 0)
    {
        if (ftruncate(fd, len) == -1)
            errExit("ftruncate error");
    }
    data = (player *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED)
        errExit("mmap");
    int n = len / sizeof(player);
    int res = 0, k = 1;
    char *name;
    do
    {
        name = readLine(fifoNames, k);
        for (int i = 0; i < n; ++i)
        {
            res = strcmp(name, data[i].fifo_name) if (res == 0)
            {
                break;
            }
        }
        k++;
    } while (res == 0);
    umask(0);
    if (mkfifo(name, mode) == -1 && errno != EEXIST)
        errExit("mkfifo error!");
    myFd = open(name, O_RDONLY);
    if (myFd == -1)
        errExit("open fifo error!");
    dummyFd = open(myFd, O_WRONLY);
    if (dummyFd == -1)
        errExit("open fifo error!");
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        errExit("signal");

    return 0;
}
