#include "main.h"
#include "helper.h"

sem_t *sem_mutex;
sem_t *sem_full1;
sem_t *sem_empty1;
sem_t *sem_full2;
sem_t *sem_empty2;
static char memoryName[50];
static clinic *biontech;
static process processInfo;

int main(int argc, char *argv[])
{
    
    args givenParams;
    checkArguments(argc, argv, &givenParams);
    biontech = getSharedMemory(givenParams);
    openSem(givenParams.bArg);
    const int LENGTH = givenParams.cArg;
    pid_t arr[LENGTH];
    createSignalHandler();
    processInfo.pid = getpid();
    processInfo.type = PARENT;
    processInfo.index = 0;
    createCitizens(biontech,arr);
    printf("Citizen Pids\n");
    for (int i = 0; i < LENGTH; i++)
    {
        printf("%ld, ",(long)arr[i]);
    }
    printf("\n");
    
    //createVaccinators(biontech);
    //createNurses(biontech);
    printf("hereeeee!\n");
    for (int i = 0; i < givenParams.tArg; i++)
    {
        for (int j = 0; j < LENGTH; j++)
        {
            printf("pid:%ld\n",(long)arr[j]);
            kill(arr[j],SIGUSR1);
        }
        //printf("%ld, ",(long)arr[i]);
    }

    reapDeadChildren();
    cleanAndExit();
    
}
clinic *getSharedMemory(args givenArgs){
    struct stat sb;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    strcpy(memoryName,"clinic_sinovac344");
    int memFd = shm_open(memoryName, O_CREAT | O_RDWR, mode);
    if (memFd == -1)
        errExit("shm_open error!");
    if (fstat(memFd, &sb) == -1)
        errExit("fstat error");
    if (ftruncate(memFd, sizeof(*biontech)) == -1)  
        errExit("ftruncate error");

    clinic *gata = (clinic *)mmap(NULL, sizeof(*biontech), PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
    if (gata == MAP_FAILED)
        errExit("mmap");
    
    gata->givenParams = givenArgs;
    gata->dose1 = 0;
    gata->dose2 = 0;
    gata->totalLeft = 2 * (givenArgs.tArg * givenArgs.cArg);
    gata->fd = safeOpen(givenArgs.iArg, O_RDWR);
    return gata;

}
void openSem(int bufferSize){
    /*create named semphores*/
    sem_mutex = sem_open("mutex344", O_CREAT, 0666, 1);
    if (sem_mutex == SEM_FAILED)
        errExit("sem_open error!");

    sem_full1 = sem_open("full344", O_CREAT, 0666, 0);
    if (sem_full1 == SEM_FAILED)
        errExit("sem_open error!");
    sem_empty1 = sem_open("empty344", O_CREAT, 0666, bufferSize);
    if (sem_empty1 == SEM_FAILED)
        errExit("sem_open error!");
    sem_full2 = sem_open("full3442", O_CREAT, 0666, 0);
    if (sem_full2 == SEM_FAILED)
        errExit("sem_open error!");
    sem_empty2 = sem_open("empty3442", O_CREAT, 0666, bufferSize);
    if (sem_empty2 == SEM_FAILED)
        errExit("sem_open error!");
}
void removeAll()
{
    printf("hereee!!\n");
    if (sem_close(sem_full1) == -1)
        errExit("sem_close");
    if (sem_close(sem_empty1) == -1)
        errExit("sem_close");
    if (sem_close(sem_full2) == -1)
        errExit("sem_close");
    if (sem_close(sem_empty2) == -1)
        errExit("sem_close");
    if (sem_close(sem_mutex) == -1)
        errExit("sem_close");
    sem_unlink("mutex344");
    sem_unlink("full344");
    sem_unlink("empty344");
    sem_unlink("full3442");
    sem_unlink("empty3442");
    shm_unlink(memoryName);
}
void createNurses(clinic *biontech){
   
    for (int i = 0; i < biontech->givenParams.nArg; i++)
    {
        
        if (fork() == 0)
        {
            /* child process*/

            processInfo.pid = getpid();
            processInfo.type = NURSE;
            processInfo.index = i;
            createSignalHandler();
            openSem(biontech->givenParams.bArg);
            nurse(biontech,&processInfo);
            cleanAndExit();
        }
        
    }
    
}
void createVaccinators(clinic *biontech){
    for (int i = 0; i < biontech->givenParams.vArg; i++)
    {
        
        if (fork() == 0)
        {
            /* child process*/

            processInfo.pid = getpid();
            processInfo.type = VACCINATOR;
            processInfo.index = i;
            createSignalHandler();
            openSem(biontech->givenParams.bArg);
            vaccinator(biontech,&processInfo);
            cleanAndExit();
        }
        
    }
}
void createCitizens(clinic *biontech,pid_t arr[]){
    for (int i = 0; i < biontech->givenParams.cArg; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            /* child process*/

            processInfo.pid = getpid();
            processInfo.type = CITIZEN;
            processInfo.index = i;
            createSignalHandler();
            createSignalHandler2();
            citizen(biontech,&processInfo);
            cleanAndExit();
        }
        else if (pid > 0)
        {
            /* parent */
            arr[i] = pid;
        }
        else
        {
            errExit("fork error!");
        }
        
    }
}
void createSignalHandler(){
    struct sigaction sa;
    sa.sa_handler = &exitHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        errExit("sigaction error!");
    }
}
void createSignalHandler2(){
    struct sigaction sa1;
    sa1.sa_handler = &wakeHandler;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa1, NULL) < 0) {
        errExit("sigaction error!");
    }
}
void exitHandler(int signal)
{
    if (signal == SIGINT)
    {
        int savedErrno = errno;
        cleanAndExit();
        errno = savedErrno;
        
    }
}
void wakeHandler(int signal)
{
    if (signal == SIGUSR1)
    {
    }
}
void cleanAndExit(){
    
    if (processInfo.type == PARENT)
    {
        reapDeadChildren();
        removeAll();
        if (close(biontech->fd) < 0)
        {
            errExit("close file error!");
        }
    }
    int unmap = munmap(biontech,sizeof(*biontech));
    if (unmap == -1)
    {
        errExit("unmap error!");
    }
    exit(EXIT_SUCCESS);
}
void reapDeadChildren(){
    pid_t childPid;
    int status;
    while ((childPid = waitpid(-1, &status, 0)) > 0);
    if (childPid == -1 && errno != ECHILD)
        errExit("waitpid"); 

}
void nurse(clinic *biontech, process *process){
    
    while (biontech->totalLeft > 0)
    {
        printf("totalLeftNurse:%d\n",biontech->totalLeft);
        if (sem_wait(sem_empty1) == -1)
            errExit("sem_wait");
        if (sem_wait(sem_empty2) == -1)
            errExit("sem_wait");
        if (sem_wait(sem_mutex) == -1)
            errExit("sem_wait");
        char vaccine = readOneChar(biontech->fd);  
        if (vaccine == '1')
        {
            biontech->dose1 = biontech->dose1 + 1;
            //biontech->totalLeft = biontech->totalLeft - 1;
            printNurseMsg(process->index,process->pid,'1',biontech);
            if (sem_post(sem_full1) == -1)
                errExit("sem_post");
        }
        else if (vaccine == '2')
        {
            biontech->dose2 = biontech->dose2 + 1;
            printNurseMsg(process->index,process->pid,'2',biontech);
            if (sem_post(sem_full2) == -1)
                errExit("sem_post");
        }
        else if (vaccine =='x')
        {
            
            break;
        }
        else{
            errExit("vaccine is wrong!!");
        }
        /*if (biontech->dose1 > 0 && biontech->dose2 > 0)
        {
            int fullPost;
            if (sem_getvalue(sem_full, &fullPost) == -1)
                errExit("sem_get");
            printf("postValue:%d\n",fullPost);
            int min = getMin(biontech->dose1,biontech->dose2);
            if ((min - fullPost) >= 1)
            {
                printf("now POSTINGGG.....!!\n");
                if (sem_post(sem_full) == -1){
                    errExit("sem_post"); 
                }

            }
            
        }*/
        if (sem_post(sem_mutex) == -1)
            errExit("sem_post");
    }
    if (sem_post(sem_mutex) == -1)
        errExit("sem_post");
    if (sem_post(sem_full1) == -1)
            errExit("sem_post");
    if (sem_post(sem_full2) == -1)
            errExit("sem_post");
    printf("Nurse %d (PID=%ld) exiting...\n", process->index, (long)getpid());
    _exit(EXIT_SUCCESS);
    
}
void vaccinator(clinic *biontech, process *process){
    while (biontech->totalLeft > 0)
    {
        printf("totalLeftVaccinator:%d\n",biontech->totalLeft);
        if (sem_wait(sem_full1) == -1)
            errExit("sem_wait");
        if (sem_wait(sem_full2) == -1)
            errExit("sem_wait");
        printf("VACc GET FULL KEYY!!!\n");
        if (sem_wait(sem_mutex) == -1)
            errExit("sem_wait");
        biontech->dose1 = biontech->dose1 - 1;
        biontech->dose2 = biontech->dose2 - 1;
        biontech->totalLeft = biontech->totalLeft - 2;
        printVaccinatorMsg(process->index,process->pid,getpid());
        if (sem_post(sem_mutex) == -1)
            errExit("sem_post");
        if (sem_post(sem_empty1) == -1)
            errExit("sem_post");
        if (sem_post(sem_empty2) == -1)
            errExit("sem_post");
        
        
    }
    if (sem_post(sem_mutex) == -1)
        errExit("sem_post");
    if (sem_post(sem_empty1) == -1)
        errExit("sem_post");
    if (sem_post(sem_empty2) == -1)
        errExit("sem_post");
    printf("Vaccinator %d (PID=%ld) exiting...\n", process->index, (long)getpid());
    _exit(EXIT_SUCCESS);
    
}
void citizen(clinic *biontech, process *process){

    sigset_t sigset1;
    //sigfillset(&sigset1); 
    //sigdelset(&sigset1, SIGUSR1);
            
    int left = biontech->givenParams.tArg;
    int total = biontech->givenParams.tArg;
    while (left > 0)
    {

        sigfillset(&sigset1); 
        sigdelset(&sigset1, SIGUSR1);
        if (sigsuspend(&sigset1) == -1 && errno != EINTR)
            errExit("suspend");
        left--;
        printCitizenMsg(process->index,process->pid,total - left,biontech);
    }
    
    
}   