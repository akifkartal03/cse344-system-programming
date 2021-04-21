#include "helper.h"

static char myFifoName[50];
static char memoryName[50];
static char semphoreName[50];
static void removeAll(void)
{
    unlink(myFifoName);
    sem_unlink(semphoreName);
    sem_unlink("counter");
    sem_unlink("barrier");
    shm_unlink(memoryName);
}
void exitHandler(int signal)
{
    if (signal == SIGINT)
    {
        exit(EXIT_FAILURE);
        //exitSignal = 1;
    }
}
int main(int argc, char *argv[])
{
    /*CTRL-C signal handling with sigaction*/
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &exitHandler;
    sigaction(SIGINT, &sa, NULL);

    struct args givenParams;
    checkArguments(argc, argv, &givenParams);

    int myFd, dummyFd, reciverFd, fd, fifoNames;
    struct stat sb;
    player *data;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    sem_t *sem_id = sem_open(givenParams.mArg, O_CREAT, 0666, 1);

    if (sem_id == SEM_FAILED)
        errExit("sem_open error!");
    fifoNames = safeOpen(givenParams.fArg, O_RDWR);
    int n = getNumberOfLine(fifoNames);
    sem_t *sem_count = sem_open("counter", O_CREAT, 0666, n + 1);

    if (sem_count == SEM_FAILED)
        errExit("sem_open error!");
    sem_t *sem_barrier = sem_open("barrier", O_CREAT, 0666, 0);

    if (sem_barrier == SEM_FAILED)
        errExit("sem_open error!");

    if (sem_wait(sem_id) == -1)
        errExit("sem_wait");
    if (sem_wait(sem_count) == -1)
        errExit("sem_wait");

    fd = shm_open(givenParams.sArg, O_CREAT | O_RDWR, mode);
    if (fd == -1)
        errExit("shm_open error!");
    if (fstat(fd, &sb) == -1)
        errExit("fstat error");
    size_t len = 4096;
    if (sb.st_size == 0)
    {
        if (ftruncate(fd, len) == -1)
            errExit("ftruncate error");
    }
    data = (player *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED)
        errExit("mmap");
    int res = 0, k = 1;
    char *name;
    do
    {
        name = readLine(fifoNames, k);

        for (int i = 0; i < n; ++i)
        {

            res = strcmp(name, data[i].fifo_name);
            if (res == 0)
            {
                break;
            }
        }
        k++;
    } while (res == 0);
    k--;
    strcpy(memoryName, givenParams.sArg);
    strcpy(myFifoName, name);
    strcpy(semphoreName, givenParams.mArg);
    if (atexit(removeAll) != 0)
        errExit("atexit");

    int numberOfSwitch = atoi(givenParams.bArg);
    player info;
    strcpy(info.fifo_name, name);
    info.switches = numberOfSwitch;
    info.pot_pid = getpid();
    info.done = 1;
    memcpy(&data[k - 1], &info, sizeof(info));
    int counter;
    if (sem_getvalue(sem_count, &counter) == -1)
        errExit("sem_get");

    if (counter <= 1)
    {
        if (sem_post(sem_barrier) == -1)
            errExit("sem_post");
    }
    if (sem_post(sem_id) == -1)
        errExit("sem_post");

    if (sem_wait(sem_barrier) == -1)
        errExit("sem_wait");

    if (sem_post(sem_barrier) == -1)
        errExit("sem_post");
    umask(0);
    if (numberOfSwitch > 0)
    {
        char randFifoName[50];
        int is_found = 0;
        for (int i = 0; i < n; i++)
        {
            if (data[i].switches == 0)
            {
                strcpy(randFifoName, data[i].fifo_name);
                is_found = 1;
                break;
            }
        }
        if (!is_found)
        {
            printf("There should be at least 1 process with 0 potato!\n");
            exit(EXIT_FAILURE);
        }
        if (mkfifo(randFifoName, mode) == -1 && errno != EEXIST)
            errExit("mkfifo error!");
        reciverFd = open(randFifoName, O_WRONLY);
        if (reciverFd == -1)
            errExit("open fifo error!");
        int recFd = open(randFifoName, O_RDONLY);
        if (recFd == -1)
            errExit("open fifo error!");
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
            errExit("signal");
        printf("pid=%ld sending potato number %ld to %s; this is switch number 1\n",
               (long)getpid(), (long)getpid(), randFifoName);
        struct sender req;
        req.pid = getpid();
        strcpy(req.fifo_name, name);
        if (write(reciverFd, &req, sizeof(struct sender)) != sizeof(struct sender))
            errExit("Can't send potato!");
        if (close(reciverFd))
            errExit("close error!!");
        if (close(recFd))
            errExit("close error!!");
    }
    if (mkfifo(name, mode) == -1 && errno != EEXIST)
        errExit("mkfifo error!");
    myFd = open(name, O_RDONLY);
    if (myFd == -1)
        errExit("open fifo error!");
    dummyFd = open(name, O_WRONLY);
    if (dummyFd == -1)
        errExit("open fifo error!");
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        errExit("signal");
    while (1)
    {
        struct sender resp;
        if (read(myFd, &resp, sizeof(struct sender)) != sizeof(struct sender))
        {
            continue;
        }
        if (resp.pid == -1)
        {
            free(name);
            exit(EXIT_SUCCESS);
        }
        if (sem_wait(sem_id) == -1)
            errExit("sem_wait");
        int index;
        for (int i = 0; i < n; i++)
        {
            if (data[i].pot_pid == resp.pid)
            {
                index = i;
            }
        }
        printf("pid=%ld receiving potato number %ld from %s\n", (long)getpid(), (long)resp.pid, resp.fifo_name);

        data[index].switches = data[index].switches - 1;
        data[index].done = data[index].done + 1;

        if (sem_post(sem_id) == -1)
            errExit("sem_post");
        if (data[index].switches > 0)
        {
            int rnd;
            do
            {
                rnd = getRandom(getNumberOfLine(fifoNames));
            } while (rnd == k);
            char *randFifoName = readLine(fifoNames, rnd);
            if (mkfifo(randFifoName, mode) == -1 && errno != EEXIST)
                errExit("mkfifo error!");
            reciverFd = open(randFifoName, O_WRONLY);
            if (reciverFd == -1)
                errExit("open fifo error!");
            int recFd = open(randFifoName, O_RDONLY);
            if (recFd == -1)
                errExit("open fifo error!");
            printf("pid=%ld sending potato number %ld to %s; this is switch number %d\n",
                   (long)getpid(), (long)resp.pid, randFifoName, data[index].done);
            struct sender req;
            req.pid = resp.pid;
            strcpy(req.fifo_name, name);
            if (write(reciverFd, &req, sizeof(struct sender)) != sizeof(struct sender))
                errExit("Can't send potato!");
            free(randFifoName);
            if (close(reciverFd))
                errExit("close error!!");
            if (close(recFd))
                errExit("close error!!");
        }
        else
        {
            printf("pid=%ld potato number %ld has cooled down\n", (long)getpid(), (long)resp.pid);
            //check last potato
            int last = 1;
            for (int i = 0; i < n; ++i)
            {
                if (data[i].switches > 0)
                {
                    last = 0;
                    i = n;
                }
            }
            if (last)
            {

                for (int i = 1; i <= n; i++)
                {
                    if (i != k)
                    {
                        char *fifoName = readLine(fifoNames, i);
                        if (mkfifo(fifoName, mode) == -1 && errno != EEXIST)
                            errExit("mkfifo error!");
                        reciverFd = open(fifoName, O_WRONLY);
                        if (reciverFd == -1)
                            errExit("open fifo error!");
                        int recFd = open(fifoName, O_RDONLY);
                        if (recFd == -1)
                            errExit("open fifo error!");
                        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
                            errExit("signal");
                        struct sender req;
                        req.pid = -1;
                        if (write(reciverFd, &req, sizeof(struct sender)) != sizeof(struct sender))
                            errExit("Can't send exit message!");
                        free(fifoName);
                    }
                }
                free(name);
                break;
            }
        }
    }

    return 0;
}
