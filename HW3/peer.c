#include "helper.h"

static char myFifoName[50];
static char memoryName[50];
static char semphoreName[50];
static void removeAll(void)
{
    unlink(myFifoName);
    sem_unlink(semphoreName);
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
    
    //printf("contuine!!!\n");
    int myFd, dummyFd, reciverFd, fd, fifoNames;
    struct stat sb;
    player *data;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    sem_t *sem_id = sem_open(givenParams.mArg, O_CREAT, 0666, 1);
    if (sem_id == SEM_FAILED)
        errExit("sem_open error!");

    if (sem_wait(sem_id) == -1)
        errExit("sem_wait");
    
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
    k--;
    strcpy(memoryName,givenParams.sArg);
    strcpy(myFifoName,name);
    strcpy(semphoreName,givenParams.mArg);
    if(atexit(removeAll) != 0)
        errExit("atexit");
    
    printf("name:%s\n",name);
    printf("k:%d\n",k);
    printf("run_pid=%ld\n",(long)getpid());
    int numberOfSwitch = atoi(givenParams.bArg);
    player info;
    strcpy(info.fifo_name, name);
    info.switches = numberOfSwitch;
    info.pot_pid = getpid();
    info.done = 1;
    memcpy(&data[k - 1], &info, sizeof(info));
    if (sem_post(sem_id) == -1)
        errExit("sem_post");
    
    umask(0);
    if (numberOfSwitch > 0)
    {
        
        int rnd;
        do
        {
            rnd = getRandom(getNumberOfLine(fifoNames));
        } while (rnd == k);
        printf("ICERDE LOOPDAN SONRA!!!!\n");
        char *randFifoName = readLine(fifoNames, rnd);
        if (mkfifo(randFifoName, mode) == -1 && errno != EEXIST)
            errExit("mkfifo error!");
        reciverFd = open(randFifoName, O_WRONLY);
        if (reciverFd == -1)
            errExit("open fifo error!");
        //printf("randfile:%s\n", randFifoName);
        int recFd = open(randFifoName, O_RDONLY);
        if (recFd == -1)
            errExit("open fifo error!");
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
            errExit("signal");
        printf("pid=%ld sending potato number %ld to %s; this is switch number 1\n",
               (long)getpid(), (long)getpid(), randFifoName);
        struct sender req;
        req.pid = getpid();
        strcpy(req.fifo_name,name);
        printf("ICERDE LOOPDAN SONRA!!!!\n");
        if (write(reciverFd, &req, sizeof(struct sender)) != sizeof(struct sender))
            errExit("Can't send potato!");
        free(randFifoName);
        printf("ICERDE WRITEEE SONRA!!!!\n");
        if (close(reciverFd))
            errExit("close error!!");
        if (close(recFd))
            errExit("close error!!");

        
        
    }
    printf("DISARDAAAAA!!!\n");
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
        printf("wait önuu\n");
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
        //player updatedInfo;
        
        data[index].switches = data[index].switches - 1;
        data[index].done = data[index].done + 1;
        printf("switch_pid: %ld\n",(long)data[index].pot_pid);
        printf("switch_number: %d\n",data[index].switches);
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
            req.pid =resp.pid;
            strcpy(req.fifo_name,name);
            if (write(reciverFd, &req, sizeof(struct sender)) != sizeof(struct sender))
                errExit("Can't send potato!");
            free(randFifoName);
            if (close(reciverFd))
                errExit("close error!!");
            if (close(recFd))
                errExit("close error!!");
        }
        else{
            printf("pid=%ld potato number %ld has cooled down\n",(long)getpid(), (long)resp.pid);
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
                
                printf("this is last!!!\n");
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
