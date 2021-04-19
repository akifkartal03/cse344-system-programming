#include "helper.h"

int main(int argc, char *argv[])
{
    /*CTRL-C signal handling with sigaction*/
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &exitHandler;
    sigaction(SIGINT, &sa, NULL);
    char test[20] = "deneme";
    sem_t *sem_id1 = sem_open(test, O_CREAT, 0666, 1);
    if (sem_id1 == SEM_FAILED)
    {
        errExit("sem_open error!");
    }
    if (sem_wait(sem_id1) == -1)
        errExit("sem_wait");

    struct args givenParams;
    checkArguments(argc, argv, &givenParams);
    if (sem_post(sem_id1) == -1)
        errExit("sem_post");
    printf("contuine!!!\n");
    int myFd, dummyFd, reciverFd, fd, fifoNames;
    //char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct stat sb;
    player *data;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    sem_t *sem_id = sem_open(givenParams.mArg, O_CREAT, 0666, 1);
    if (sem_id == SEM_FAILED)
    {
        errExit("sem_open error!");
    }
    fifoNames = safeOpen(givenParams.fArg, O_RDWR);
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
    int n = getNumberOfLine(fifoNames);
    int res = 0, k = 1;
    char *name;
    if (sem_wait(sem_id) == -1)
        errExit("sem_wait");
    do
    {
        name = readLine(fifoNames, k);
        //printf("name:%s\n",name);
        for (int i = 0; i < n; ++i)
        {
            //printf("data: %s\n",data[i].fifo_name);
            res = strcmp(name, data[i].fifo_name);
            //printf("res:%d\n",res);
            if (res == 0)
            {
                break;
            }
        }
        k++;
    } while (res == 0);
    //printf("name:%s\n",name);
    //printf("k:%d\n",k);
    int numberOfSwitch = atoi(givenParams.bArg);
    player info;
    strcpy(info.fifo_name, name);
    info.switches = numberOfSwitch;
    info.pot_pid = getpid();
    info.done = 1;
    memcpy(&data[k - 1], &info, sizeof(info));
    if (sem_post(sem_id) == -1)
        errExit("sem_post");
    if (numberOfSwitch > 0)
    {
        //int rnd = getRandom(getNumberOfLine(fifoNames));
        char *randFifoName = readLine(fifoNames, getRandom(getNumberOfLine(fifoNames)));
        printf("randfile:%s\n", randFifoName);
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
        req.dataID = k - 1;
        sprintf(req.msg, "pid=%ld sending potato number %ld to %s; this is switch number 1\n",
                (long)getpid(), (long)getpid(), randFifoName);
        if (write(reciverFd, &req, sizeof(struct sender)) != sizeof(struct sender))
            errExit("Can't send potato!");
        free(randFifoName);
    }
    umask(0);
    if (mkfifo(name, mode) == -1 && errno != EEXIST)
        errExit("mkfifo error!");
    myFd = open(name, O_RDONLY); // kendi fifosunu açıyor
    if (myFd == -1)
        errExit("open fifo error!");
    dummyFd = open(name, O_WRONLY);
    if (dummyFd == -1)
        errExit("open fifo error!");
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        errExit("signal");
    struct sender resp;
    while (1)
    {
        if (read(myFd, &resp, sizeof(struct sender)) != sizeof(struct sender))
        {
            continue;
        }
        if (strcmp(resp.msg, "exit") == 0)
        {
            unlink(name);
            free(name);
            exit(EXIT_SUCCESS);
        }
        printf("pid=%ld receiving potato number %ld from %s\n", (long)getpid(), (long)resp.pid, name);
        //player updatedInfo;
        if (sem_wait(sem_id) == -1)
            errExit("sem_wait");
        data[resp.dataID].switches = data[resp.dataID].switches - 1;
        data[resp.dataID].done = data[resp.dataID].done + 1;
        if (sem_post(sem_id) == -1)
            errExit("sem_post");
        if (data[resp.dataID].switches > 0)
        {
            char *randFifoName = readLine(fifoNames, getRandom(getNumberOfLine(fifoNames)));
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
            printf("pid=%ld sending potato number %ld to %s; this is switch number %d\n",
                   (long)getpid(), (long)resp.pid, randFifoName, data[resp.dataID].done);
            struct sender req;
            req.pid = resp.pid;
            req.dataID = resp.dataID;
            sprintf(req.msg, "pid=%ld sending potato number %ld to %s; this is switch number %d\n",
                    (long)getpid(), (long)getpid(), randFifoName, data[resp.dataID].done);
            if (write(reciverFd, &req, sizeof(struct sender)) != sizeof(struct sender))
                errExit("Can't send potato!");
            free(randFifoName);
        }
        else
        {
            //check last potato
            int last = 1;
            for (int i = 0; i < n; ++i)
            {
                if (data[i].switches > 0)
                {
                    last = 0;
                    break;
                }
            }
            if (last)
            {
                free(name);
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
                        req.pid = getpid();
                        req.dataID = 0;
                        sprintf(req.msg, "exit");
                        if (write(reciverFd, &req, sizeof(struct sender)) != sizeof(struct sender))
                            errExit("Can't send exit message!");
                        free(fifoName);
                    }
                }
                unlink(name);
                free(name);
                exit(EXIT_SUCCESS);
            }
        }
    }

    return 0;
}
