#include "helper.h"

/*global names to remove*/
static char myFifoName[50];
static char memoryName[50];
static char semphoreName[50];
sem_t *sem_id;
sem_t *sem_count;
sem_t *sem_barrier;
sem_t *sem_fifo_barrier;
/*remove function before exit*/
static void removeAll(void)
{
    if (sem_close(sem_id) == -1)
        errExit("sem_close");
    if (sem_close(sem_count) == -1)
        errExit("sem_close");
    if (sem_close(sem_barrier) == -1)
        errExit("sem_close");
    if (sem_close(sem_fifo_barrier) == -1)
        errExit("sem_close");
    unlink(myFifoName);
    sem_unlink(semphoreName);
    sem_unlink("counter");
    sem_unlink("barrier");
    sem_unlink("fifo_barrier");
    shm_unlink(memoryName);
}

int main(int argc, char *argv[])
{
    /*CTRL-C signal handling with sigaction*/
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &exitHandler;
    sigaction(SIGINT, &sa, NULL);

    /*Check arguments*/
    srand(time(0));
    struct args givenParams;
    checkArguments(argc, argv, &givenParams);

    /*create important variables*/
    int myFd, dummyFd, reciverFd, fd, fifoNames,tempFd;
    struct stat sb;
    player *data;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;

    /*open fifo names file and get number of lines*/
    fifoNames = safeOpen(givenParams.fArg, O_RDWR);
    int n = getNumberOfLine(fifoNames);

    /*open given semaphore*/
    sem_id = sem_open(givenParams.mArg, O_CREAT, 0666, 1);
    if (sem_id == SEM_FAILED)
        errExit("sem_open error!");

    /*define other semaphores to make synchronization between process*/
    sem_count = sem_open("counter", O_CREAT, 0666, n + 1);
    if (sem_count == SEM_FAILED)
        errExit("sem_open error!");
    sem_barrier = sem_open("barrier", O_CREAT, 0666, 0);
    if (sem_barrier == SEM_FAILED)
        errExit("sem_open error!");
    sem_fifo_barrier = sem_open("fifo_barrier", O_CREAT, 0666, 0);
    if (sem_fifo_barrier == SEM_FAILED)
        errExit("sem_open error!");

    /*enter critical region with given semaphore*/
    if (sem_wait(sem_id) == -1)
        errExit("sem_wait");

    /*decrement counter semaphore*/    
    if (sem_wait(sem_count) == -1)
        errExit("sem_wait");

    /*open shared memory*/
    fd = shm_open(givenParams.sArg, O_CREAT | O_RDWR, mode);
    if (fd == -1)
        errExit("shm_open error!");

    /*adjust it's size if needed*/    
    if (fstat(fd, &sb) == -1)
        errExit("fstat error");
    size_t len = 4096 + (n*sizeof(player));
    if (sb.st_size == 0)
    {
        if (ftruncate(fd, len) == -1)
            errExit("ftruncate error");
    }
    if (exitSignal)
    {
        printf("exiting....\n");
        exit(EXIT_FAILURE);
    }

    /*attach itself to the shared memory segment*/
    data = (player *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED)
        errExit("mmap");

    /*find unique fifo name using shared memory*/    
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
                free(name);
                break;
            }
        }
        k++;
        if (exitSignal)
        {
            printf("exiting....\n");
            exit(EXIT_FAILURE);
        }
    } while (res == 0);
    k--;

    /*after finding name adjust variables*/
    strcpy(memoryName, givenParams.sArg);
    strcpy(myFifoName, name);
    strcpy(semphoreName, givenParams.mArg);

    /*save remove method*/
    if (atexit(removeAll) != 0)
        errExit("atexit");

    /*get real name of fifo(aliveli etc.)*/    
    char *ch;
    char temp_name[50];
    char realt_name[30];
    strcpy(temp_name,name);
    ch = strtok(temp_name, "/");
    while (ch != NULL) {
        strcpy(realt_name,ch);
        ch = strtok(NULL, "/");
    }

    /*add potato to the shared memory*/
    int numberOfSwitch = atoi(givenParams.bArg);
    player info;
    strcpy(info.fifo_name, name);
    strcpy(info.real_name, realt_name);
    info.switches = numberOfSwitch;
    info.pot_pid = getpid();
    info.done = 1;
    info.is_opened = 0;
    memcpy(&data[k - 1], &info, sizeof(info));

    /*create fifo*/
    umask(0);
    if (mkfifo(name, mode) == -1 && errno != EEXIST)
        errExit("mkfifo error!");

    /*get counter semaphore value*/    
    int counter;
    if (sem_getvalue(sem_count, &counter) == -1)
        errExit("sem_get");

    /*
    * if this is not first running process,
    * check is there a waiting fifo to open
    * and open it in write mode.
    */
    if (counter != n)
    {
        for (int i = 0; i < n; i++)
        {
            if ((data[i].is_opened == 0) && (strlen(data[i].fifo_name) > 0) &&  (data[i].pot_pid != getpid())
                         &&   (data[i].pot_pid > 1)  && (data[i].done == 1))
            {
                tempFd = open(data[i].fifo_name, O_WRONLY);
                if (tempFd == -1)
                    errExit("open fifo error!");
                data[i].is_opened = 1;
            }
            if (exitSignal)
            {
                printf("exiting....\n");
                exit(EXIT_FAILURE);
            }
        }  
    }
    /*if this is last process running, free the semophore
    * so that other process can contiune
    */
    if (counter <= 1)
    {
        if (sem_post(sem_fifo_barrier) == -1)
            errExit("sem_post");
    }
    /*free given critical region semaphore*/
    if (sem_post(sem_id) == -1)
        errExit("sem_post");
    /*open your fifo both reading and wiriting mode*/
    myFd = open(name, O_RDONLY);
    if (myFd == -1)
        errExit("open fifo error!");
    dummyFd = open(name, O_WRONLY);
    if (dummyFd == -1)
        errExit("open fifo error!");
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        errExit("signal");

    /*if you can open your fifo wait for last process
    * free the semaphore so you can contiune.
    */    
     if (sem_wait(sem_fifo_barrier) == -1)
        errExit("sem_wait");

    /*aftercontiune, post again for other process*/
    if (sem_post(sem_fifo_barrier) == -1)
        errExit("sem_post");

    /*
    * and then lastly check is there any waiting process 
    * to open it's fifo, if any open it
    */    
    for (int i = 0; i < n; i++)
    {
        if ((data[i].is_opened == 0) && (strlen(data[i].fifo_name) > 0) &&  (data[i].pot_pid != getpid())
                        &&   (data[i].pot_pid > 1)  && (data[i].done == 1))
        {
            tempFd = open(data[i].fifo_name, O_WRONLY);
            if (tempFd == -1)
                errExit("open fifo error!");
            data[i].is_opened = 1;
        }
        if (exitSignal)
        {
            printf("exiting....\n");
            exit(EXIT_FAILURE);
        }
    }   
    
    if (exitSignal)
    {
        printf("exiting....\n");
        exit(EXIT_FAILURE);
    }
    
    /*
    * check all process have finished
    *their opening and initilaziton
    */
    if (counter <= 1)
    {
        if (sem_post(sem_barrier) == -1)
            errExit("sem_post");
    }
    
    /*
    * if there is a process still is not finished
    * it's opening and initilaziton then wait.
    */
    if (sem_wait(sem_barrier) == -1)
        errExit("sem_wait");

    /*
    * if all process have finished it's opening 
    * and initilaziton first post to others and contiune.
    */
    if (sem_post(sem_barrier) == -1)
        errExit("sem_post");

    printf("\n");
    if (numberOfSwitch > 0)
    {
        /*
        * if the process has potato send it another one 
        */
        int rnd;
        do
        {
            rnd = getRandom(getNumberOfLine(fifoNames));
        } while (rnd == k);
        char *randFifoName = readLine(fifoNames, rnd);
        strcpy(temp_name,randFifoName);
        ch = strtok(temp_name, "/");
        char randRealName[30];
        while (ch != NULL) {
            strcpy(randRealName,ch);
            ch = strtok(NULL, "/");
        }
        
        if (exitSignal)
        {
            printf("exiting....\n");
            exit(EXIT_FAILURE);
        }
        reciverFd = open(randFifoName, O_WRONLY);
        if (reciverFd == -1)
            errExit("open fifo error!");
        printf("pid=%ld sending potato number %ld to %s; this is switch number 1\n",
               (long)getpid(), (long)getpid(), randRealName);
        struct sender req;
        req.pid = getpid();
        strcpy(req.fifo_name, realt_name);
        if (write(reciverFd, &req, sizeof(struct sender)) != sizeof(struct sender))
            errExit("Can't send potato!");
        if (close(reciverFd))
            errExit("close error!!");
        free(randFifoName);
    }
     /*
    * if the process has not a potato or already send it
    * then wait until one of them come. 
    */
    while (1)
    {
        if (exitSignal)
        {
            printf("exiting....\n");
            exit(EXIT_FAILURE);
        }
        struct sender resp;
        /*wait here by reading fifo*/
        if (read(myFd, &resp, sizeof(struct sender)) != sizeof(struct sender))
        {
            continue;
        }
        /*exit message*/
        if (resp.pid == -1)
        {
            free(name);
            exit(EXIT_SUCCESS);
        }
         /*critical region update shared memory*/
        if (sem_wait(sem_id) == -1)
            errExit("sem_wait");
        int index;
        for (int i = 0; i < n; i++)
        {
            if (data[i].pot_pid == resp.pid)
            {
                index = i;
            }
            if (exitSignal)
            {
                printf("exiting....\n");
                exit(EXIT_FAILURE);
            }
        }
        printf("pid=%ld receiving potato number %ld from %s\n", (long)getpid(), (long)resp.pid, resp.fifo_name);

        data[index].switches = data[index].switches - 1;
        data[index].done = data[index].done + 1;

        if (sem_post(sem_id) == -1)
            errExit("sem_post");
        /*exit critical region and switch number*/    
        if (data[index].switches > 0)
        {
            /*send it again*/  
            int rnd;
            do
            {
                rnd = getRandom(getNumberOfLine(fifoNames));
            } while (rnd == k);
            char *randFifoName = readLine(fifoNames, rnd);
            strcpy(temp_name,randFifoName);
            ch = strtok(temp_name, "/");
            char r_name[30];
            while (ch != NULL) {
                strcpy(r_name,ch);
                ch = strtok(NULL, "/");
            }
            if (exitSignal)
            {
                printf("exiting....\n");
                exit(EXIT_FAILURE);
            }

            reciverFd = open(randFifoName, O_WRONLY);
            if (reciverFd == -1)
                errExit("open fifo error!");
            printf("pid=%ld sending potato number %ld to %s; this is switch number %d\n",
                   (long)getpid(), (long)resp.pid, r_name, data[index].done);
            struct sender req;
            req.pid = resp.pid;
            strcpy(req.fifo_name, realt_name);
            if (write(reciverFd, &req, sizeof(struct sender)) != sizeof(struct sender))
                errExit("Can't send potato!");
            if (close(reciverFd))
                errExit("close error!!");
            free(randFifoName);
        }
        else
        {
            if (exitSignal)
            {
                printf("exiting....\n");
                exit(EXIT_FAILURE);
            }
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
                /*it was last so send exit message all other process*/  
                for (int i = 1; i <= n; i++)
                {
                    if (i != k)
                    {
                        char *fifoName = readLine(fifoNames, i);
                        reciverFd = open(fifoName, O_WRONLY);
                        if (reciverFd == -1)
                            errExit("open fifo error!");
                        struct sender req;
                        req.pid = -1;
                        if (write(reciverFd, &req, sizeof(struct sender)) != sizeof(struct sender))
                            errExit("Can't send exit message!");
                        if (close(reciverFd))
                            errExit("close error!!");
                        free(fifoName);
                    }
                    if (exitSignal)
                    {
                        printf("exiting....\n");
                        exit(EXIT_FAILURE);
                    }
                }
                /*exit itself*/  
                 
                free(name);
                break;
            }
        }
    }

    return 0;
}
